set_version("1.0.0")
add_rules("mode.debug", "mode.release")

includes("@builtin/xpack")

toolchain("luckfox")
    set_kind("standalone")
    set_sdkdir("/home/song/luckfox-pico/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf")
toolchain_end()

local LIBGPHOTO2_PATH = "/home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2.so"
local LIBGPHOTO2_PORT_PATH = "/home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2_port.so"
local CAMLIBS_PATH = "/home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2/2.5.31/*"
local IOLIBS_PATH = "/home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2_port/0.12.2/*"
local USB_LIBS_PATH = "/home/song/arm_libgphoto2/arm_install_libusb-compat-0.1.8/lib/libusb.so"
local USB_LIBS_1_0_PATH = "/home/song/arm_libgphoto2/arm_install_libusb/lib/libusb-1.0.so"
local LIBXMLE_2_PATH = "/home/song/arm_libgphoto2/arm_install_libxml2/lib/libxml2.so.2"

-- 设置 SDK
set_toolchains("luckfox")

target("DSLR_plug-in")
    -- 编译成可执行文件
    set_kind("binary")

    -- libgphoto2 库的配置
    add_includedirs("/home/song/arm_libgphoto2/install_libgphoto2/include")

    -- 设置链接库
    add_links(LIBGPHOTO2_PATH)
    add_links(LIBGPHOTO2_PORT_PATH)
    add_links(USB_LIBS_PATH)
    add_links(USB_LIBS_1_0_PATH)
    add_links("/home/song/arm_libgphoto2/install_libgphoto2/lib/libgphoto2/2.5.31/ptp2.so")
    add_links("/home/song/arm_libgphoto2/arm_install_libxml2/lib/libxml2.so")
    add_links("pthread")

    -- 设置 SDK
    set_toolchains("luckfox")

    -- 源文件
    add_files("src/*.c", "src/uart/src/*.c|main.c", "src/oled/*.c|main.c", "src/oled/i2c/*.c", "src/oled/Fonts/*.c")

    -- c11 标准
    set_languages("c11", "cxx11")

target_end()

target("luckfox_uart")
    set_kind("binary")
    add_files("./src/uart/src/*.c")
target_end()

xpack("DSLR_PLOG_IN")
    set_formats("zip")
    set_title("DSL_PLOG_IN")
    set_author("SongZihui")
    set_description("你的单反相机的副驾驶。")
    set_homepage("https://szhwho.top")
    set_licensefile("LICENSE")
    add_targets("DSLR_plug-in")
    add_sourcefiles("(src/**)")

    after_installcmd(function (package, batchcmds)
        batchcmds:mkdir(package:installdir("lib"))
        batchcmds:cp(LIBGPHOTO2_PATH, package:installdir("lib"), {rootdir = "src"})
        batchcmds:cp(LIBGPHOTO2_PORT_PATH, package:installdir("lib"), {rootdir = "src"})
        batchcmds:cp(USB_LIBS_PATH, package:installdir("lib"), {rootdir = "src"})
        batchcmds:cp(USB_LIBS_1_0_PATH, package:installdir("lib"), {rootdir = "src"})
        batchcmds:cp(LIBXMLE_2_PATH, package:installdir("lib"), {rootdir = "src"})
        batchcmds:cp("./install.sh", package:installdir(""), {rootdir = "src"})
        batchcmds:cp("./uninstall.sh", package:installdir(""), {rootdir = "src"})
        batchcmds:mkdir(package:installdir("lib/camlibs"))
        batchcmds:mkdir(package:installdir("lib/iolibs"))
        batchcmds:cp(CAMLIBS_PATH, package:installdir("lib/camlibs"), {rootdir = "src"})
        batchcmds:cp(IOLIBS_PATH, package:installdir("lib/iolibs"), {rootdir = "src"})
    end)

    after_uninstallcmd(function (package, batchcmds)
        batchcmds:rmdir(package:installdir("lib"))
    end)