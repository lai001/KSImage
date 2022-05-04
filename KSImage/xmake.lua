includes("../../Foundation/Foundation")

add_requires("bgfx")
add_requires("spdlog")
-- add_requires("stb")
add_requires("glm")

rule("KSImage.deps")
    on_load(function (target)
        local oldir = os.cd(target:scriptdir())
        import("devel.git")
        if os.exists("../../Foundation") == false then
            git.clone("https://github.com/lai001/Foundation.git", {branch = "main", outputdir = "../../Foundation"})
        end
        os.cd(oldir)
    end)


rule("KSImage.CompileShader")
    on_load(function (target)
        local oldir = os.cd(path.join(target:scriptdir(), "Shader"))
        if os.exists("Shader.tmp") == false then
            os.mkdir("Shader.tmp")
        end
        local shaderFiles = table.join(os.files("*.vert"), os.files("*.frag"))
        for _, filepath in ipairs(shaderFiles) do
            local target = format("Shader.tmp/%s", path.filename(filepath))
            local extension = path.extension(filepath)
            local type = "fragments"
            local profile = "ps_5_0"
            if extension == ".vert" then
                type = "vertex"
                profile = "vs_5_0"
            end
            if os.exists(target) == false then
                local command = format("../Vendor/shaderc.exe -f %s -o %s --type %s --platform windows -p %s -O 3 --varyingdef varying.def.sc -i .", filepath, target, type, profile)
                os.run(command)
            end
        end
        os.cd(oldir)
    end)

rule("KSImage.Copy")
    after_build(function (target)
        local oldir = os.cd(target:scriptdir())
        local shaderPath = path.join(target:targetdir(), "Shader")
        if os.exists(shaderPath) then
            os.rmdir(shaderPath)
        end
        os.cp("Shader/Shader.tmp", shaderPath)

        local VariableShaderDir = path.join(target:targetdir(), "VariableShader")
        if os.exists(VariableShaderDir) == false then
            os.mkdir(VariableShaderDir)
        end
        local bgfx_shaderPath = path.join(VariableShaderDir, "bgfx_shader.sh")
        if os.exists(bgfx_shaderPath) == false then
            os.cp("Shader/bgfx_shader.sh", bgfx_shaderPath)
        end
        local binPath = path.join(VariableShaderDir, "shaderc.exe")  
        if os.exists(binPath) == false then
            os.cp("Vendor/shaderc.exe" , binPath)
        end
        local varyingdefPath = path.join(VariableShaderDir, "varying.def.sc")  
        if os.exists(varyingdefPath) == false then
            os.cp("Shader/varying.def.sc" , varyingdefPath)
        end

        os.cd(oldir)
    end)

rule("KSImage.Copy.shaderc")
    on_load(function (target)
        local bgfx_pkgs = target:pkgs()["bgfx"]
        local shadercBinDir = path.join(bgfx_pkgs:installdir(), "bin")
        local shadercBin = path.join(shadercBinDir, "shaderc*.exe")
        os.cp(shadercBin, path.join(target:scriptdir(), "Vendor/shaderc.exe"))
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
    add_rules("KSImage.Copy.shaderc")
    add_rules("KSImage.CompileShader")
    add_rules("KSImage.Copy")
    add_rules("KSImage.deps")
    add_packages("bgfx")
    add_packages("spdlog")
    -- add_packages("stb")
    add_packages("glm")
    add_deps("Foundation")
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