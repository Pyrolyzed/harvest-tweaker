local AUTHOR_NAME = "Pyrolyzed"
local PRODUCT_NAME = "HarvestTweaks"
local BEAUTIFUL_NAME = "Harvest Tweaks SKSE"

-- include subprojects
includes("lib/commonlibsse-ng")

-- set project constants
set_project(PRODUCT_NAME)
set_version("1.0.0")
set_license("Unlicense")
set_languages("c++23")
set_warnings("allextra")

-- add common rules
add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

-- define targets
target(PRODUCT_NAME)
    add_rules("commonlibsse-ng.plugin", {
        name = PRODUCT_NAME,
        author = AUTHOR_NAME,
        description = BEAUTIFUL_NAME,
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
