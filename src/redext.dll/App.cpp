#include <stdafx.hpp>
#include <App.hpp>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <DevConsole.hpp>

REDext::App* REDext::App::Get()
{
    static App app;
    return &app;
}

void REDext::App::Init(HMODULE aModule)
{
    // Make sure there is a "REDext" directory in the "Documents".
    auto [err, docsPath] = GetDocumentsPath();
    if (err)
    {
        MessageBox(nullptr, L"Could not get the path to 'Documents' folder.", L"REDext", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    docsPath /= L"REDext";
    if (!std::filesystem::exists(docsPath) && !std::filesystem::create_directories(docsPath))
    {
        return;
    }

#ifdef _DEBUG
    DevConsole::Alloc();
#endif

    // Initialize the logger.
    InitializeLogger(docsPath);

    spdlog::info(L"REDext started");
    spdlog::debug(L"Base address is {:#x}", reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr)));

    m_pluginManager.Init(aModule, docsPath);
}

void REDext::App::Run()
{
}

void REDext::App::Shutdown()
{
    m_pluginManager.Shutdown();
    spdlog::shutdown();

#ifdef _DEBUG
    DevConsole::Free();
#endif
}

REDext::PluginManager* REDext::App::GetPluginManager()
{
    return &m_pluginManager;
}

std::tuple<std::error_code, std::filesystem::path> REDext::App::GetDocumentsPath()
{
    wchar_t* pathRaw = nullptr;
    std::filesystem::path path;

    if (FAILED(SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &pathRaw)))
    {
        return { std::error_code(ERROR_PATH_NOT_FOUND, std::system_category()), "" };
    }

    path = pathRaw;
    CoTaskMemFree(pathRaw);

    return { { std::error_code() }, path };
}

void REDext::App::InitializeLogger(std::filesystem::path aRoot)
{
    auto console = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
    auto file = std::make_shared<spdlog::sinks::basic_file_sink_st>(aRoot / L"game.log", true);

    spdlog::sinks_init_list sinks = { console, file };

    auto logger = std::make_shared<spdlog::logger>("", sinks);
    spdlog::set_default_logger(logger);

#ifdef _DEBUG
    logger->flush_on(spdlog::level::trace);
    spdlog::set_level(spdlog::level::trace);
#else
    logger->flush_on(spdlog::level::info);
    spdlog::set_level(spdlog::level::info);
#endif
}
