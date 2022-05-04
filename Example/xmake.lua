includes("../../Foundation/Foundation")
includes("../KSImage")
add_requires("bgfx")
add_requires("spdlog")
add_requires("stb")
add_requires("glm")

task("deps")
    on_run(function ()
        import("devel.git")
        if os.exists("../../Foundation") == false then
            git.clone("https://github.com/lai001/Foundation.git", {branch = "main", outputdir = "../../Foundation"})
        end
        if os.exists("./Vendor/imgui") == false then
            local oldir = os.cd("Vendor")
            git.clone("https://github.com/ocornut/imgui.git", {branch = "master", outputdir = "imgui"})
            git.checkout("v1.87", {repodir = "imgui"})
            os.cd(oldir)
        end
    end)
    set_menu {
        usage = "xmake deps",
        description = "download deps"
    }

rule("Example.Copy")
    after_build(function (target)
        os.cp("Resource", target:targetdir())
    end)

rule("Example.Clean")    
    after_clean(function (target)
        os.rm(path.join(target:targetdir(), "Resource"))
    end)

rule("Example.FixCompileError")
    on_load(function (target)
        local bgfx_pkgs = target:pkgs()["bgfx"]
        local headerDir = path.join(bgfx_pkgs:installdir(), "include/compat/msvc")
        target:add("includedirs", headerDir)
    end)

target("Example")
    set_kind("binary")
    set_languages("c++17")
    add_files("./**.cpp|Vendor/**.cpp")
    add_headerfiles("./**.h|Vendor/**.h")
    add_headerfiles("./**.hpp|Vendor/**.hpp")
    add_includedirs("./")
    add_includedirs("Vendor/imgui", "Vendor/imgui/backends")
    add_syslinks("d3d11")
    add_rules("mode.debug", "mode.release")
    add_rules("Example.Copy")
    add_rules("Example.Clean")
    add_rules("Example.FixCompileError")
    add_deps("Foundation")
    add_deps("ImGui")
    add_deps("KSImage")
    add_packages("bgfx")
    add_packages("spdlog")
    add_packages("stb")
    add_packages("glm")

target("ImGui")
    set_kind("static")
    set_languages("c++17")
    add_includedirs("Vendor/imgui")
    add_files("Vendor/imgui/*.cpp")
    add_files("Vendor/imgui/backends/imgui_impl_dx11.cpp")
    add_files("Vendor/imgui/backends/imgui_impl_win32.cpp")
    add_headerfiles("Vendor/imgui/*.h")
    add_headerfiles("Vendor/imgui/backends/imgui_impl_win32.h")
    add_headerfiles("Vendor/imgui/backends/imgui_impl_dx11.h")