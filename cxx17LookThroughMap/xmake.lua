toolchain("gcc-11.3.0")
    set_kind("standalone")
    set_sdkdir("/usr/local/gcc-11.3.0")
toolchain_end()

add_rules("mode.debug", "mode.releasedbg")
set_languages("c99", "cxx20")
set_toolchains("gcc-11.3.0")
-- 内部封装了 cl、clang、gcc 的异常选项：/EHsc、-fexceptions 
set_exceptions("cxx")

target("demo")
    set_kind("binary")
    add_files("*.cpp")

    -- 自动生成 compile_commands.json 帮助代码补全跳转
    after_build(function (target)
        import("core.base.task")
        task.run("project", {kind = "compile_commands", outputdir = ".vscode"})
    end)
target_end()