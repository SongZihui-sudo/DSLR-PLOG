add_rules("mode.debug", "mode.release")

toolchain("luckfox")
    set_kind("standalone")
    set_sdkdir("/home/song/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf")
toolchain_end()

target("luckfox-uart")
    set_kind("binary")
    add_files("src/*.c")
    set_toolchains("luckfox")

