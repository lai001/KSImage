includes("../../KSRenderEngine/KSRenderEngine")

add_requires("spdlog")
add_requires("glm")

rule("KSImage.deps")
    on_load(function (target)
        local oldir = os.cd(target:scriptdir())
        import("devel.git")
        if os.exists("../../Foundation") == false then
            git.clone("https://github.com/lai001/Foundation.git", {branch = "main", outputdir = "../../Foundation"})
        end
        if os.exists("../../KSRenderEngine") == false then
            git.clone("https://github.com/lai001/KSRenderEngine.git", {branch = "main", outputdir = "../../KSRenderEngine"})
        end
        os.cd(oldir)
    end)

target("KSImage")
    set_kind("$(kind)")
    set_languages("c++17")
    add_files("./**.cpp")
    add_headerfiles("./**.h", "./**.hpp")
    add_includedirs("include/KSImage")
    add_includedirs("include", {interface = true})
    add_syslinks("d3d11")
    add_rules("mode.debug", "mode.release")
    add_rules("KSImage.deps")
    add_packages("spdlog")
    add_packages("glm")
    add_deps("Foundation")
    add_deps("KSRenderEngine")
    add_defines("GLM_FORCE_XYZW_ONLY=1")
    if is_kind("shared") and is_plat("windows") then
        add_defines("KSImage_BUILD_DLL_EXPORT")
    end
    on_config(function (target)
        import("core.project.project")
        for _targetname, _target in pairs(project.targets()) do
            local depsType = type(_target:get("deps"))
            local deps = nil;
            if depsType == "table" then
                deps = _target:get("deps")
            elseif depsType == "string" then
                deps = {_target:get("deps")}
            end
            if deps and table.contains(deps, "KSImage") and target:kind() == "shared" and target:is_plat("windows") then
                _target:add("defines", "KSImage_DLL")
            end
        end
    end)

target("Foundation")

target("KSRenderEngine")