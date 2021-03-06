includes("../../Foundation/Foundation")
includes("../KSImage")
add_requires("spdlog")
add_requires("glm")
add_requires("openimageio")

rule("Example.deps")
    on_load(function (target)
        local oldir = os.cd(target:scriptdir())
        import("devel.git")
        if os.exists("./Vendor/imgui") == false then
            local oldir = os.cd("Vendor")
            git.clone("https://github.com/ocornut/imgui.git", {branch = "master", outputdir = "imgui"})
            git.checkout("v1.87", {repodir = "imgui"})
            os.cd(oldir)
        end
        os.cd(oldir)
    end)

rule("Example.Copy")
    after_build(function (target)
        local resourceDir = path.join(target:targetdir(), "Resource")
        local shaderDir = path.join(resourceDir, "Shader")
        if os.exists(resourceDir) == false then
            os.mkdir(resourceDir)
        end
        if os.exists(shaderDir) == false then
            os.mkdir(shaderDir)
        end
        os.cp("Shader/*.hlsl", shaderDir)
        os.cp("Resource/*.jpg", resourceDir)
        os.cp("Resource/*.png", resourceDir)
    end)

rule("Example.Clean")    
    after_clean(function (target)
        os.rm(path.join(target:targetdir(), "Resource"))
    end)

target("Example")
    set_kind("binary")
    set_languages("c++17")
    add_files("src/**.cpp")
    add_headerfiles("src/**.h", "src/**.hpp")
    add_headerfiles("Shader/*.hlsl")
    add_includedirs("src")
    add_syslinks("d3d11")
    add_rules("mode.debug", "mode.release", "Example.deps")
    add_rules("Example.Copy")
    add_rules("Example.Clean")
    add_deps("Foundation")
    add_deps("ImGui")
    add_deps("KSImage")
    add_packages("spdlog")
    add_packages("glm")
    add_packages("openimageio")
    add_defines("GLM_FORCE_XYZW_ONLY=1")

target("ImGui")
    set_kind("static")
    set_languages("c++17")
    add_rules("mode.debug", "mode.release", "Example.deps")
    add_includedirs("Vendor/imgui", { public = true })
    add_includedirs("Vendor", { interface = true })
    add_files("Vendor/imgui/*.cpp")
    add_files("Vendor/imgui/backends/imgui_impl_dx11.cpp")
    add_files("Vendor/imgui/backends/imgui_impl_win32.cpp")
    add_headerfiles("Vendor/imgui/*.h")
    add_headerfiles("Vendor/imgui/backends/imgui_impl_win32.h")
    add_headerfiles("Vendor/imgui/backends/imgui_impl_dx11.h")