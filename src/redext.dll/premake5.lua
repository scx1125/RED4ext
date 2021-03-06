project("REDext.Dll")
    targetname("REDext")
    targetdir(redext.paths.build("bin"))

    kind("SharedLib")
    language("C++")
    pchheader("stdafx.hpp")
    pchsource("stdafx.cpp")

    defines(
    {
        redext.project.defines("spdlog")
    })

    includedirs(
    {
        ".",
        redext.project.includes("RenHook"),
        redext.project.includes("spdlog")
    })

    files(
    {
        "**.cpp",
        "**.hpp"
    })

    links(
    {
        redext.project.links("RenHook"),
        redext.project.links("spdlog")
    })
