#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "R9IE01",  # USA Rev 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds NonMatching (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
parser.add_argument(
    "--develop",
    dest="develop",
    action="store_true",
    help="builds equivalent objects and code with DEVELOP flag",
)

args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-1"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.7.1"
config.objdiff_tag = "v3.4.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.0-beta.5"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Progress configuration
config.progress_all = False
config.progress_use_fancy = True
config.progress_code_fancy_frac = 30
config.progress_code_fancy_item = "ship parts"
config.progress_data_fancy_frac = 100
config.progress_data_fancy_item = "Pikmin"

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    # "-W all",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-enc SJIS",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    "-i include/stl",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    f"-DDTK_CONFIG_NONMATCHING={config.non_matching:d}",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.extend(
        ["-DNDEBUG=1", "-w off"]
    )  # no I DO not want to talk about my car's extended warranty.

# DEVELOP flags
if args.develop:
    cflags_base.extend(["-DDEVELOP=1"])

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")


# JAudio flags
cflags_jaudio = [
    "-nodefaults",
    "-proc gekko",
    '-pragma "scheduling 7400"',
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    # "-W all",
    "-O4,s",
    "-inline off",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse, readonly",
    "-enc SJIS",
    "-i include",
    "-i include/stl",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    f"-DDTK_CONFIG_NONMATCHING={config.non_matching:d}",
    "-common on",
    "-func_align 32",
    "-lang c++",
    "-DNDEBUG=1",
    "-w off",
    "-use_lmw_stmw on",
]

# Game code flags
cflags_pikmin = [
    *cflags_base,
    "-char unsigned",
    "-str reuse, readonly",
    "-use_lmw_stmw on",
]

# EGG library flags
cflags_egg = [
    *cflags_base,
    "-enc SJIS",
    "-ipa file",
    "-str reuse,readonly",
    "-use_lmw_stmw on",
]

# NW4R library flags
cflags_nw4r = [
    *cflags_base,
    "-enc SJIS",
    "-fp_contract off",
    "-ipa file",
]

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]


config.linker_version = "GC/3.0a5.2"


# Helper function for Dolphin libraries
def RevolutionLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/3.0a5.2",
        "cflags": [*cflags_base, "-str noreadonly", "-ipa file"],
        "progress_category": "sdk",
        "objects": objects,
    }


Matching = True  # Object matches and should be linked
NonMatching = False  # Object does not match and should not be linked
Equivalent = (
    config.non_matching
)  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    {
        "lib": "sysBootup",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "sysBootup.cpp"),
        ],
    },
    {
        "lib": "jaudio",
        "cflags": cflags_jaudio,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "jaudio",
        "objects": [
            Object(NonMatching, "jaudio/dummyprobe.c"),
            Object(NonMatching, "jaudio/memory.c"),
            Object(NonMatching, "jaudio/aictrl.c"),
            Object(NonMatching, "jaudio/sample.c"),
            Object(NonMatching, "jaudio/dummyrom.c"),
            Object(NonMatching, "jaudio/audiothread.c"),
            Object(NonMatching, "jaudio/streamctrl.c"),
            Object(NonMatching, "jaudio/dspbuf.c"),
            Object(NonMatching, "jaudio/cpubuf.c"),
            Object(NonMatching, "jaudio/playercall.c"),
            Object(NonMatching, "jaudio/dvdthread.c"),
            Object(NonMatching, "jaudio/rate.c"),
            Object(NonMatching, "jaudio/audiomesg.c"),
            Object(NonMatching, "jaudio/stackchecker.c"),
            Object(NonMatching, "jaudio/dspproc.c"),
            Object(NonMatching, "jaudio/dsptask.c"),
            Object(NonMatching, "jaudio/osdsp.c"),
            Object(NonMatching, "jaudio/osdsp_task.c"),
            Object(NonMatching, "jaudio/driverinterface.c"),
            Object(NonMatching, "jaudio/dspdriver.c"),
            Object(NonMatching, "jaudio/dspinterface.c"),
            Object(NonMatching, "jaudio/fxinterface.c"),
            Object(NonMatching, "jaudio/bankread.c"),
            Object(NonMatching, "jaudio/waveread.c"),
            Object(NonMatching, "jaudio/connect.c"),
            Object(NonMatching, "jaudio/tables.c"),
            Object(NonMatching, "jaudio/bankdrv.c"),
            Object(NonMatching, "jaudio/random.c"),
            Object(NonMatching, "jaudio/aramcall.c"),
            Object(NonMatching, "jaudio/ja_calc.c"),
            Object(NonMatching, "jaudio/fat.c"),
            Object(NonMatching, "jaudio/cmdstack.c"),
            Object(NonMatching, "jaudio/virload.c"),
            Object(NonMatching, "jaudio/heapctrl.c"),
            Object(NonMatching, "jaudio/jammain_2.c"),
            Object(NonMatching, "jaudio/midplay.c"),
            Object(NonMatching, "jaudio/noteon.c"),
            Object(NonMatching, "jaudio/seqsetup.c"),
            Object(NonMatching, "jaudio/centcalc.c"),
            Object(NonMatching, "jaudio/jamosc.c"),
            Object(NonMatching, "jaudio/oneshot.c"),
            Object(NonMatching, "jaudio/interface.c"),
            Object(NonMatching, "jaudio/verysimple.c"),
            Object(NonMatching, "jaudio/app_inter.c"),
            Object(NonMatching, "jaudio/pikiinter.c"),
            Object(NonMatching, "jaudio/piki_player.c"),
            Object(NonMatching, "jaudio/piki_bgm.c"),
            Object(NonMatching, "jaudio/piki_scene.c"),
            Object(NonMatching, "jaudio/pikidemo.c"),
            Object(NonMatching, "jaudio/file_seq.c"),
            Object(NonMatching, "jaudio/cmdqueue.c"),
            Object(NonMatching, "jaudio/filter3d.c"),
            Object(NonMatching, "jaudio/syncstream.c"),
            Object(NonMatching, "jaudio/bankloader.c"),
            Object(NonMatching, "jaudio/interleave.c"),
            Object(NonMatching, "jaudio/pikiseq.c"),
        ],
    },
    RevolutionLib(
        "aralt",
        [
            Object(NonMatching, "aralt/aralt.c"),
        ],
    ),
    {
        "lib": "TRK_Hollywood_Revolution",
        "mw_version": "GC/3.0a5.2",
        "progress_category": "sdk",
        "cflags": [
            *cflags_runtime,
            "-rostr",
            "-sdata 0",
            "-sdata2 0",
            "-pool off",
            "-inline on,noauto",
        ],
        "objects": [
            Object(NonMatching, "TRK_Hollywood_Revolution/mainloop.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/nubevent.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/nubinit.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/msg.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/msgbuf.c"),
            Object(
                NonMatching,
                "TRK_Hollywood_Revolution/serpoll.c",
                extra_cflags=["-sdata 8"],
            ),
            Object(NonMatching, "TRK_Hollywood_Revolution/errno.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/usr_put.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/dispatch.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/msghndlr.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/support.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/mutex_TRK.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/notify.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/flush_cache.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/mem_TRK.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/string_TRK.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/targimpl.c"),
            Object(
                NonMatching,
                "TRK_Hollywood_Revolution/targsupp.c",
                extra_cflags=["-func_align 32"],
            ),
            Object(NonMatching, "TRK_Hollywood_Revolution/mpc_7xx_603e.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/mslsupp.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/__exception.s"),
            Object(NonMatching, "TRK_Hollywood_Revolution/dolphin_trk.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/main_TRK.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/dolphin_trk_glue.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/targcont.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/target_options.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/UDP_Stubs.c"),
            Object(
                NonMatching,
                "TRK_Hollywood_Revolution/main.c",
                extra_cflags=["-sdata 8"],
            ),
            Object(NonMatching, "TRK_Hollywood_Revolution/CircleBuffer.c"),
            Object(NonMatching, "TRK_Hollywood_Revolution/MWCriticalSection_gc.c"),
        ],
    },
    {
        "lib": "MSL_C.PPCEABI.bare.H",
        "mw_version": "GC/3.0a5.2",
        "progress_category": "sdk",
        "cflags": [
            *cflags_base,
            "-fp_contract on",
            "-inline auto,deferred",
            "-str pool,readonly",
        ],
        "objects": [
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/alloc.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/ansi_files.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/abort_exit.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/errno.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/ansi_fp.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/arith.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/buffer_io.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/ctype.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/wctype.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/locale.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/direct_io.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/file_io.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/FILE_POS.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/mbstring.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/mem.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/mem_funcs.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/math_api.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/misc_io.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/printf.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/rand.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/scanf.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/string.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/strtold.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/strtoul.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/wstring.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/wchar_io.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/uart_console_io_gcn.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/float.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/math_sun.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/math_double.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/extras.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_asin.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_atan2.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_fmod.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_log.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_log10.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_pow.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_rem_pio2.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/k_cos.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/k_rem_pio2.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/k_sin.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/k_tan.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_atan.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_copysign.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_cos.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_floor.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_frexp.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_ldexp.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_modf.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_sin.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/s_tan.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_asin.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_atan2.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_fmod.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_log10.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_pow.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/e_sqrt.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/math_ppc.c"),
            Object(NonMatching, "MSL_C/PPCEABI/bare/H/w_sqrt.c"),
        ],
    },
    RevolutionLib(
        "ai",
        [
            Object(NonMatching, "ai/ai.c"),
        ],
    ),
    RevolutionLib(
        "ax",
        [
            Object(NonMatching, "ax/AX.c"),
            Object(NonMatching, "ax/AXAlloc.c"),
            Object(NonMatching, "ax/AXAux.c"),
            Object(NonMatching, "ax/AXCL.c"),
            Object(NonMatching, "ax/AXOut.c"),
            Object(NonMatching, "ax/AXSPB.c"),
            Object(NonMatching, "ax/AXVPB.c"),
            Object(NonMatching, "ax/AXProf.c"),
            Object(NonMatching, "ax/AXFXReverbHi.c"),
            Object(NonMatching, "ax/AXFXReverbHiExp.c"),
            Object(NonMatching, "ax/DSPCode.c"),
        ],
    ),
    RevolutionLib(
        "axfx",
        [
            Object(NonMatching, "axfx/AXFXHooks.c"),
        ],
    ),
    RevolutionLib(
        "base",
        [
            Object(NonMatching, "base/PPCArch.c"),
        ],
    ),
    RevolutionLib(
        "db",
        [
            Object(NonMatching, "db/db.c"),
        ],
    ),
    RevolutionLib(
        "dsp",
        [
            Object(NonMatching, "dsp/dsp.c"),
            Object(NonMatching, "dsp/dsp_debug.c"),
            Object(NonMatching, "dsp/dsp_task.c"),
        ],
    ),
    RevolutionLib(
        "dvd",
        [
            Object(NonMatching, "dvd/dvdfs.c"),
            Object(NonMatching, "dvd/dvd.c"),
            Object(NonMatching, "dvd/dvdqueue.c"),
            Object(NonMatching, "dvd/dvderror.c"),
            Object(NonMatching, "dvd/dvdidutils.c"),
            Object(NonMatching, "dvd/dvdFatal.c"),
            Object(NonMatching, "dvd/dvdDeviceError.c"),
            Object(NonMatching, "dvd/dvd_broadway.c"),
        ],
    ),
    RevolutionLib(
        "exi",
        [
            Object(NonMatching, "exi/EXIBios.c"),
            Object(NonMatching, "exi/EXIUart.c"),
            Object(NonMatching, "exi/EXICommon.c"),
        ],
    ),
    RevolutionLib(
        "gx",
        [
            Object(NonMatching, "gx/GXInit.c"),
            Object(NonMatching, "gx/GXFifo.c"),
            Object(NonMatching, "gx/GXAttr.c"),
            Object(NonMatching, "gx/GXMisc.c"),
            Object(NonMatching, "gx/GXGeometry.c"),
            Object(NonMatching, "gx/GXFrameBuf.c"),
            Object(NonMatching, "gx/GXLight.c"),
            Object(NonMatching, "gx/GXTexture.c"),
            Object(NonMatching, "gx/GXBump.c"),
            Object(NonMatching, "gx/GXTev.c"),
            Object(NonMatching, "gx/GXPixel.c"),
            Object(NonMatching, "gx/GXDisplayList.c"),
            Object(NonMatching, "gx/GXTransform.c"),
            Object(NonMatching, "gx/GXPerf.c"),
        ],
    ),
    RevolutionLib(
        "mtx",
        [
            Object(NonMatching, "mtx/mtx.c"),
            Object(NonMatching, "mtx/mtxvec.c"),
            Object(NonMatching, "mtx/mtx44.c"),
            Object(NonMatching, "mtx/mtx44vec.c"),
            Object(NonMatching, "mtx/vec.c"),
        ],
    ),
    RevolutionLib(
        "NdevExi2A",
        [
            Object(NonMatching, "NdevExi2A/DebuggerDriver.c"),
            Object(NonMatching, "NdevExi2A/exi2.c"),
        ],
    ),
    RevolutionLib(
        "os",
        [
            Object(NonMatching, "os/OS.c"),
            Object(NonMatching, "os/OSAlarm.c"),
            Object(NonMatching, "os/OSAlloc.c"),
            Object(NonMatching, "os/OSArena.c"),
            Object(NonMatching, "os/OSAudioSystem.c"),
            Object(NonMatching, "os/OSCache.c"),
            Object(NonMatching, "os/OSContext.c"),
            Object(NonMatching, "os/OSError.c"),
            Object(NonMatching, "os/OSExec.c"),
            Object(NonMatching, "os/OSFatal.c"),
            Object(NonMatching, "os/OSFont.c"),
            Object(NonMatching, "os/OSInterrupt.c"),
            Object(NonMatching, "os/OSLink.c"),
            Object(NonMatching, "os/OSMessage.c"),
            Object(NonMatching, "os/OSMemory.c"),
            Object(NonMatching, "os/OSMutex.c"),
            Object(NonMatching, "os/OSReboot.c"),
            Object(NonMatching, "os/OSReset.c"),
            Object(NonMatching, "os/OSRtc.c"),
            Object(NonMatching, "os/OSSync.c"),
            Object(NonMatching, "os/OSThread.c"),
            Object(NonMatching, "os/OSTime.c"),
            Object(NonMatching, "os/OSUtf.c"),
            Object(NonMatching, "os/OSIpc.c"),
            Object(NonMatching, "os/OSStateTM.c"),
            Object(NonMatching, "os/OSPlayRecord.c"),
            Object(NonMatching, "os/OSStateFlags.c"),
            Object(NonMatching, "os/OSNet.c"),
            Object(NonMatching, "os/OSNandbootInfo.c"),
            Object(NonMatching, "os/OSPlayTime.c"),
            Object(NonMatching, "os/OSLaunch.c"),
            Object(NonMatching, "os/init/__start.c"),
            Object(NonMatching, "os/init/__ppc_eabi_init.cpp"),
            Object(NonMatching, "os/__ppc_eabi_init.c"),
        ],
    ),
    RevolutionLib(
        "pad",
        [
            Object(NonMatching, "pad/Padclamp.c"),
            Object(NonMatching, "pad/Pad.c"),
        ],
    ),
    RevolutionLib(
        "si",
        [
            Object(NonMatching, "si/SIBios.c"),
            Object(NonMatching, "si/SISamplingRate.c"),
        ],
    ),
    RevolutionLib(
        "vi",
        [
            Object(NonMatching, "vi/vi.c"),
            Object(NonMatching, "vi/i2c.c"),
            Object(NonMatching, "vi/vi3in1.c"),
        ],
    ),
    RevolutionLib(
        "thp",
        [
            Object(NonMatching, "thp/THPDec.c"),
            Object(NonMatching, "thp/THPAudio.c"),
        ],
    ),
    RevolutionLib(
        "tpl",
        [
            Object(NonMatching, "tpl/TPL.c"),
        ],
    ),
    RevolutionLib(
        "usb",
        [
            Object(NonMatching, "usb/usb.c"),
        ],
    ),
    RevolutionLib(
        "wenc",
        [
            Object(NonMatching, "wenc/wenc.c"),
        ],
    ),
    RevolutionLib(
        "wpad",
        [
            Object(NonMatching, "wpad/WPAD.c"),
            Object(NonMatching, "wpad/WPADHIDParser.c"),
            Object(NonMatching, "wpad/WPADEncrypt.c"),
            Object(NonMatching, "wpad/debug_msg.c"),
        ],
    ),
    RevolutionLib(
        "wud",
        [
            Object(NonMatching, "wud/WUD.c"),
            Object(NonMatching, "wud/WUDHidHost.c"),
            Object(NonMatching, "wud/debug_msg.c"),
        ],
    ),
    RevolutionLib(
        "arc",
        [
            Object(NonMatching, "arc/arc.c"),
        ],
    ),
    RevolutionLib(
        "bte",
        [
            Object(NonMatching, "bte/gki_buffer.c"),
            Object(NonMatching, "bte/gki_time.c"),
            Object(NonMatching, "bte/gki_ppc.c"),
            Object(NonMatching, "bte/hcisu_h2.c"),
            Object(NonMatching, "bte/uusb_ppc.c"),
            Object(NonMatching, "bte/bte_hcisu.c"),
            Object(NonMatching, "bte/bte_init.c"),
            Object(NonMatching, "bte/bte_logmsg.c"),
            Object(NonMatching, "bte/bte_main.c"),
            Object(NonMatching, "bte/btu_task1.c"),
            Object(NonMatching, "bte/bd.c"),
            Object(NonMatching, "bte/bta_sys_conn.c"),
            Object(NonMatching, "bte/bta_sys_main.c"),
            Object(NonMatching, "bte/ptim.c"),
            Object(NonMatching, "bte/utl.c"),
            Object(NonMatching, "bte/bta_dm_cfg.c"),
            Object(NonMatching, "bte/bta_dm_act.c"),
            Object(NonMatching, "bte/bta_dm_api.c"),
            Object(NonMatching, "bte/bta_dm_main.c"),
            Object(NonMatching, "bte/bta_dm_pm.c"),
            Object(NonMatching, "bte/bta_hh_act.c"),
            Object(NonMatching, "bte/bta_hh_api.c"),
            Object(NonMatching, "bte/bta_hh_main.c"),
            Object(NonMatching, "bte/bta_hh_utils.c"),
            Object(NonMatching, "bte/btm_acl.c"),
            Object(NonMatching, "bte/btm_dev.c"),
            Object(NonMatching, "bte/btm_devctl.c"),
            Object(NonMatching, "bte/btm_discovery.c"),
            Object(NonMatching, "bte/btm_inq.c"),
            Object(NonMatching, "bte/btm_main.c"),
            Object(NonMatching, "bte/btm_pm.c"),
            Object(NonMatching, "bte/btm_sco.c"),
            Object(NonMatching, "bte/btm_sec.c"),
            Object(NonMatching, "bte/btu_hcif.c"),
            Object(NonMatching, "bte/btu_init.c"),
            Object(NonMatching, "bte/wbt_ext.c"),
            Object(NonMatching, "bte/gap_api.c"),
            Object(NonMatching, "bte/gap_conn.c"),
            Object(NonMatching, "bte/gap_utils.c"),
            Object(NonMatching, "bte/hcicmds.c"),
            Object(NonMatching, "bte/hidd_api.c"),
            Object(NonMatching, "bte/hidd_conn.c"),
            Object(NonMatching, "bte/hidd_mgmt.c"),
            Object(NonMatching, "bte/hidd_pm.c"),
            Object(NonMatching, "bte/hidh_api.c"),
            Object(NonMatching, "bte/hidh_conn.c"),
            Object(NonMatching, "bte/l2c_api.c"),
            Object(NonMatching, "bte/l2c_csm.c"),
            Object(NonMatching, "bte/l2c_link.c"),
            Object(NonMatching, "bte/l2c_main.c"),
            Object(NonMatching, "bte/l2c_utils.c"),
            Object(NonMatching, "bte/port_api.c"),
            Object(NonMatching, "bte/port_rfc.c"),
            Object(NonMatching, "bte/port_utils.c"),
            Object(NonMatching, "bte/rfc_l2cap_if.c"),
            Object(NonMatching, "bte/rfc_mx_fsm.c"),
            Object(NonMatching, "bte/rfc_port_fsm.c"),
            Object(NonMatching, "bte/rfc_port_if.c"),
            Object(NonMatching, "bte/rfc_ts_frames.c"),
            Object(NonMatching, "bte/rfc_utils.c"),
            Object(NonMatching, "bte/sdp_api.c"),
            Object(NonMatching, "bte/sdp_db.c"),
            Object(NonMatching, "bte/sdp_discovery.c"),
            Object(NonMatching, "bte/sdp_main.c"),
            Object(NonMatching, "bte/sdp_server.c"),
            Object(NonMatching, "bte/sdp_utils.c"),
        ],
    ),
    RevolutionLib(
        "enc",
        [
            Object(NonMatching, "enc/encutility.c"),
            Object(NonMatching, "enc/encjapanese.c"),
        ],
    ),
    RevolutionLib(
        "euart",
        [
            Object(NonMatching, "euart/euart.c"),
        ],
    ),
    RevolutionLib(
        "fs",
        [
            Object(NonMatching, "fs/fs.c"),
        ],
    ),
    RevolutionLib(
        "ipc",
        [
            Object(NonMatching, "ipc/ipcMain.c"),
            Object(NonMatching, "ipc/ipcclt.c"),
            Object(NonMatching, "ipc/memory.c"),
            Object(NonMatching, "ipc/ipcProfile.c"),
        ],
    ),
    RevolutionLib(
        "kpad",
        [
            Object(NonMatching, "kpad/KPAD.c"),
        ],
    ),
    RevolutionLib(
        "mem",
        [
            Object(NonMatching, "mem/mem_heapCommon.c"),
            Object(NonMatching, "mem/mem_expHeap.c"),
            Object(NonMatching, "mem/mem_allocator.c"),
            Object(NonMatching, "mem/mem_list.c"),
        ],
    ),
    RevolutionLib(
        "nand",
        [
            Object(NonMatching, "nand/nand.c"),
            Object(NonMatching, "nand/NANDOpenClose.c"),
            Object(NonMatching, "nand/NANDCore.c"),
            Object(NonMatching, "nand/NANDCheck.c"),
            Object(NonMatching, "nand/NANDLogging.c"),
        ],
    ),
    RevolutionLib(
        "sc",
        [
            Object(NonMatching, "sc/scsystem.c"),
            Object(NonMatching, "sc/scapi.c"),
            Object(NonMatching, "sc/scapi_prdinfo.c"),
        ],
    ),
    {
        "lib": "sysCommon",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "sysCommon/atx.cpp"),
            Object(NonMatching, "sysCommon/ayuStack.cpp"),
            Object(NonMatching, "sysCommon/baseApp.cpp"),
            Object(NonMatching, "sysCommon/camera.cpp"),
            Object(NonMatching, "sysCommon/cmdStream.cpp"),
            Object(NonMatching, "sysCommon/controller.cpp"),
            Object(NonMatching, "sysCommon/graphics.cpp"),
            Object(NonMatching, "sysCommon/grLight.cpp"),
            Object(NonMatching, "sysCommon/id32.cpp"),
            Object(NonMatching, "sysCommon/matMath.cpp"),
            Object(NonMatching, "sysCommon/node.cpp"),
            Object(NonMatching, "sysCommon/shapeBase.cpp"),
            Object(NonMatching, "sysCommon/shpLightFlares.cpp"),
            Object(NonMatching, "sysCommon/shpObjColl.cpp"),
            Object(NonMatching, "sysCommon/shpRoutes.cpp"),
            Object(NonMatching, "sysCommon/stdSystem.cpp"),
            Object(NonMatching, "sysCommon/stream.cpp"),
            Object(NonMatching, "sysCommon/streamBufferedInput.cpp"),
            Object(NonMatching, "sysCommon/string.cpp"),
            Object(NonMatching, "sysCommon/sysMath.cpp"),
            Object(NonMatching, "sysCommon/timers.cpp"),
        ],
    },
    {
        "lib": "sysDolphin",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "sysDolphin/dgxGraphics.cpp"),
            Object(NonMatching, "sysDolphin/gameApp.cpp"),
            Object(NonMatching, "sysDolphin/sysNew.cpp"),
            Object(NonMatching, "sysDolphin/system.cpp"),
            Object(NonMatching, "sysDolphin/texture.cpp"),
            Object(NonMatching, "sysDolphin/controllerMgr.cpp"),
        ],
    },
    {
        "lib": "plugPikiColin",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiColin/animMgr.cpp"),
            Object(NonMatching, "plugPikiColin/cardSelect.cpp"),
            Object(NonMatching, "plugPikiColin/cardutil.cpp"),
            Object(NonMatching, "plugPikiColin/cinePlayer.cpp"),
            Object(NonMatching, "plugPikiColin/dayMgr.cpp"),
            Object(NonMatching, "plugPikiColin/dynsimulator.cpp"),
            Object(NonMatching, "plugPikiColin/game.cpp"),
            Object(NonMatching, "plugPikiColin/gameExit.cpp"),
            Object(NonMatching, "plugPikiColin/gameflow.cpp"),
            Object(NonMatching, "plugPikiColin/gamePrefs.cpp"),
            Object(NonMatching, "plugPikiColin/gameSetup.cpp"),
            Object(NonMatching, "plugPikiColin/gauges.cpp"),
            Object(NonMatching, "plugPikiColin/genMapObject.cpp"),
            Object(NonMatching, "plugPikiColin/gui.cpp"),
            Object(NonMatching, "plugPikiColin/introGame.cpp"),
            Object(NonMatching, "plugPikiColin/lightPool.cpp"),
            Object(NonMatching, "plugPikiColin/mapMgr.cpp"),
            Object(NonMatching, "plugPikiColin/mapSelect.cpp"),
            Object(NonMatching, "plugPikiColin/memoryCard.cpp"),
            Object(NonMatching, "plugPikiColin/moviePlayer.cpp"),
            Object(NonMatching, "plugPikiColin/movSample.cpp"),
            Object(NonMatching, "plugPikiColin/newPikiGame.cpp"),
            Object(NonMatching, "plugPikiColin/ninLogo.cpp"),
            Object(NonMatching, "plugPikiColin/parameters.cpp"),
            Object(NonMatching, "plugPikiColin/plugPiki.cpp"),
            Object(NonMatching, "plugPikiColin/titles.cpp"),
        ],
    },
    {
        "lib": "THPSimple",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "thp",
        "objects": [
            Object(NonMatching, "THPSimple.c"),
        ],
    },
    {
        "lib": "plugPikiKando",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiKando/testSoundSection.cpp"),
            Object(NonMatching, "plugPikiKando/ufoAnim.cpp"),
            Object(NonMatching, "plugPikiKando/ufoItem.cpp"),
            Object(NonMatching, "plugPikiKando/updateMgr.cpp"),
            Object(NonMatching, "plugPikiKando/uteffect.cpp"),
            Object(NonMatching, "plugPikiKando/utkando.cpp"),
            Object(NonMatching, "plugPikiKando/viewPiki.cpp"),
            Object(NonMatching, "plugPikiKando/weedsItem.cpp"),
            Object(NonMatching, "plugPikiKando/workObject.cpp"),
            Object(NonMatching, "plugPikiKando/actor.cpp"),
            Object(NonMatching, "plugPikiKando/actorAI.cpp"),
            Object(NonMatching, "plugPikiKando/actorMgr.cpp"),
            Object(NonMatching, "plugPikiKando/aiAction.cpp"),
            Object(NonMatching, "plugPikiKando/aiActions.cpp"),
            Object(NonMatching, "plugPikiKando/aiAttack.cpp"),
            Object(NonMatching, "plugPikiKando/aiBoMake.cpp"),
            Object(NonMatching, "plugPikiKando/aiBore.cpp"),
            Object(NonMatching, "plugPikiKando/aiBou.cpp"),
            Object(NonMatching, "plugPikiKando/aiBreakWall.cpp"),
            Object(NonMatching, "plugPikiKando/aiBridge.cpp"),
            Object(NonMatching, "plugPikiKando/aiCarry.cpp"),
            Object(NonMatching, "plugPikiKando/aiChase.cpp"),
            Object(NonMatching, "plugPikiKando/aiConstants.cpp"),
            Object(NonMatching, "plugPikiKando/aiCrowd.cpp"),
            Object(NonMatching, "plugPikiKando/aiDecoy.cpp"),
            Object(NonMatching, "plugPikiKando/aiEnter.cpp"),
            Object(NonMatching, "plugPikiKando/aiEscape.cpp"),
            Object(NonMatching, "plugPikiKando/aiExit.cpp"),
            Object(NonMatching, "plugPikiKando/aiFormation.cpp"),
            Object(NonMatching, "plugPikiKando/aiFree.cpp"),
            Object(NonMatching, "plugPikiKando/aiGoto.cpp"),
            Object(NonMatching, "plugPikiKando/aiGuard.cpp"),
            Object(NonMatching, "plugPikiKando/aiKinoko.cpp"),
            Object(NonMatching, "plugPikiKando/aiMine.cpp"),
            Object(NonMatching, "plugPikiKando/aiPerf.cpp"),
            Object(NonMatching, "plugPikiKando/aiPick.cpp"),
            Object(NonMatching, "plugPikiKando/aiPickCreature.cpp"),
            Object(NonMatching, "plugPikiKando/aiPullout.cpp"),
            Object(NonMatching, "plugPikiKando/aiPush.cpp"),
            Object(NonMatching, "plugPikiKando/aiPut.cpp"),
            Object(NonMatching, "plugPikiKando/aiRandomBoid.cpp"),
            Object(NonMatching, "plugPikiKando/aiRescue.cpp"),
            Object(NonMatching, "plugPikiKando/aiRope.cpp"),
            Object(NonMatching, "plugPikiKando/aiShoot.cpp"),
            Object(NonMatching, "plugPikiKando/aiStone.cpp"),
            Object(NonMatching, "plugPikiKando/aiTable.cpp"),
            Object(NonMatching, "plugPikiKando/aiTransport.cpp"),
            Object(NonMatching, "plugPikiKando/aiWatch.cpp"),
            Object(NonMatching, "plugPikiKando/aiWeed.cpp"),
            Object(NonMatching, "plugPikiKando/animPellet.cpp"),
            Object(NonMatching, "plugPikiKando/bombItem.cpp"),
            Object(NonMatching, "plugPikiKando/collInfo.cpp"),
            Object(NonMatching, "plugPikiKando/complexCreature.cpp"),
            Object(NonMatching, "plugPikiKando/conditions.cpp"),
            Object(NonMatching, "plugPikiKando/courseDebug.cpp"),
            Object(NonMatching, "plugPikiKando/cPlate.cpp"),
            Object(NonMatching, "plugPikiKando/creature.cpp"),
            Object(NonMatching, "plugPikiKando/creatureCollision.cpp"),
            Object(NonMatching, "plugPikiKando/creatureCollPart.cpp"),
            Object(NonMatching, "plugPikiKando/creatureMove.cpp"),
            Object(NonMatching, "plugPikiKando/creatureStick.cpp"),
            Object(NonMatching, "plugPikiKando/demoEvent.cpp"),
            Object(NonMatching, "plugPikiKando/demoInvoker.cpp"),
            Object(NonMatching, "plugPikiKando/dualCreature.cpp"),
            Object(NonMatching, "plugPikiKando/dynCreature.cpp"),
            Object(NonMatching, "plugPikiKando/eventListener.cpp"),
            Object(NonMatching, "plugPikiKando/fastGrid.cpp"),
            Object(NonMatching, "plugPikiKando/fishItem.cpp"),
            Object(NonMatching, "plugPikiKando/formationMgr.cpp"),
            Object(NonMatching, "plugPikiKando/gameCoreSection.cpp"),
            Object(NonMatching, "plugPikiKando/gameDemo.cpp"),
            Object(NonMatching, "plugPikiKando/gameStat.cpp"),
            Object(NonMatching, "plugPikiKando/genActor.cpp"),
            Object(NonMatching, "plugPikiKando/generator.cpp"),
            Object(NonMatching, "plugPikiKando/generatorCache.cpp"),
            Object(NonMatching, "plugPikiKando/genItem.cpp"),
            Object(NonMatching, "plugPikiKando/genMapParts.cpp"),
            Object(NonMatching, "plugPikiKando/genNavi.cpp"),
            Object(NonMatching, "plugPikiKando/genPellet.cpp"),
            Object(NonMatching, "plugPikiKando/globalShapes.cpp"),
            Object(NonMatching, "plugPikiKando/gmWin.cpp"),
            Object(NonMatching, "plugPikiKando/goalItem.cpp"),
            Object(NonMatching, "plugPikiKando/interactBattle.cpp"),
            Object(NonMatching, "plugPikiKando/interactEtc.cpp"),
            Object(NonMatching, "plugPikiKando/interactGrab.cpp"),
            Object(NonMatching, "plugPikiKando/interactPullout.cpp"),
            Object(NonMatching, "plugPikiKando/itemAI.cpp"),
            Object(NonMatching, "plugPikiKando/itemGem.cpp"),
            Object(NonMatching, "plugPikiKando/itemMgr.cpp"),
            Object(NonMatching, "plugPikiKando/itemObject.cpp"),
            Object(NonMatching, "plugPikiKando/keyConfig.cpp"),
            Object(NonMatching, "plugPikiKando/keyItem.cpp"),
            Object(NonMatching, "plugPikiKando/kio.cpp"),
            Object(NonMatching, "plugPikiKando/kmath.cpp"),
            Object(NonMatching, "plugPikiKando/kontroller.cpp"),
            Object(NonMatching, "plugPikiKando/kusaItem.cpp"),
            Object(NonMatching, "plugPikiKando/mapcode.cpp"),
            Object(NonMatching, "plugPikiKando/mapParts.cpp"),
            Object(NonMatching, "plugPikiKando/memStat.cpp"),
            Object(NonMatching, "plugPikiKando/mizuItem.cpp"),
            Object(NonMatching, "plugPikiKando/navi.cpp"),
            Object(NonMatching, "plugPikiKando/naviDemoState.cpp"),
            Object(NonMatching, "plugPikiKando/naviMgr.cpp"),
            Object(NonMatching, "plugPikiKando/naviState.cpp"),
            Object(NonMatching, "plugPikiKando/objectMgr.cpp"),
            Object(NonMatching, "plugPikiKando/objectTypes.cpp"),
            Object(NonMatching, "plugPikiKando/odoMeter.cpp"),
            Object(NonMatching, "plugPikiKando/omake.cpp"),
            Object(NonMatching, "plugPikiKando/paniItemAnimator.cpp"),
            Object(NonMatching, "plugPikiKando/panipikianimator.cpp"),
            Object(NonMatching, "plugPikiKando/paniPlantAnimator.cpp"),
            Object(NonMatching, "plugPikiKando/pelletMgr.cpp"),
            Object(NonMatching, "plugPikiKando/pelletState.cpp"),
            Object(NonMatching, "plugPikiKando/piki.cpp"),
            Object(NonMatching, "plugPikiKando/pikidoKill.cpp"),
            Object(NonMatching, "plugPikiKando/pikiheadItem.cpp"),
            Object(NonMatching, "plugPikiKando/pikiInf.cpp"),
            Object(NonMatching, "plugPikiKando/pikiInfo.cpp"),
            Object(NonMatching, "plugPikiKando/pikiMgr.cpp"),
            Object(NonMatching, "plugPikiKando/pikiState.cpp"),
            Object(NonMatching, "plugPikiKando/plantMgr.cpp"),
            Object(NonMatching, "plugPikiKando/playerState.cpp"),
            Object(NonMatching, "plugPikiKando/radarInfo.cpp"),
            Object(NonMatching, "plugPikiKando/resultFlag.cpp"),
            Object(NonMatching, "plugPikiKando/ropeCreature.cpp"),
            Object(NonMatching, "plugPikiKando/ropeItem.cpp"),
            Object(NonMatching, "plugPikiKando/routeMgr.cpp"),
            Object(NonMatching, "plugPikiKando/saiEvents.cpp"),
            Object(NonMatching, "plugPikiKando/searchSystem.cpp"),
            Object(NonMatching, "plugPikiKando/seConstants.cpp"),
            Object(NonMatching, "plugPikiKando/seedItem.cpp"),
            Object(NonMatching, "plugPikiKando/seMgr.cpp"),
            Object(NonMatching, "plugPikiKando/simpleAI.cpp"),
            Object(NonMatching, "plugPikiKando/smartPtr.cpp"),
            Object(NonMatching, "plugPikiKando/soundMgr.cpp"),
        ],
    },
    {
        "lib": "plugPikiNakata",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiNakata/taijudgementactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taikinoko.cpp"),
            Object(NonMatching, "plugPikiNakata/taimessageactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taimizinko.cpp"),
            Object(NonMatching, "plugPikiNakata/taimotionactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taimoveactions.cpp"),
            Object(NonMatching, "plugPikiNakata/tainapkid.cpp"),
            Object(NonMatching, "plugPikiNakata/taiotimoti.cpp"),
            Object(NonMatching, "plugPikiNakata/taipalm.cpp"),
            Object(NonMatching, "plugPikiNakata/taireactionactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taishell.cpp"),
            Object(NonMatching, "plugPikiNakata/taiswallow.cpp"),
            Object(NonMatching, "plugPikiNakata/taitimeractions.cpp"),
            Object(NonMatching, "plugPikiNakata/taiwaitactions.cpp"),
            Object(NonMatching, "plugPikiNakata/teki.cpp"),
            Object(NonMatching, "plugPikiNakata/tekianimationmanager.cpp"),
            Object(NonMatching, "plugPikiNakata/tekibteki.cpp"),
            Object(NonMatching, "plugPikiNakata/tekiconditions.cpp"),
            Object(NonMatching, "plugPikiNakata/tekievent.cpp"),
            Object(NonMatching, "plugPikiNakata/tekiinteraction.cpp"),
            Object(NonMatching, "plugPikiNakata/tekimgr.cpp"),
            Object(NonMatching, "plugPikiNakata/tekinakata.cpp"),
            Object(NonMatching, "plugPikiNakata/tekinteki.cpp"),
            Object(NonMatching, "plugPikiNakata/tekiparameters.cpp"),
            Object(NonMatching, "plugPikiNakata/tekipersonality.cpp"),
            Object(NonMatching, "plugPikiNakata/tekistrategy.cpp"),
            Object(NonMatching, "plugPikiNakata/genteki.cpp"),
            Object(NonMatching, "plugPikiNakata/nakatacode.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibfunction.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibgeometry3d.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibgeometry.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibgraphics.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibmath.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibspline.cpp"),
            Object(NonMatching, "plugPikiNakata/nlibsystem.cpp"),
            Object(NonMatching, "plugPikiNakata/panianimator.cpp"),
            Object(NonMatching, "plugPikiNakata/panipikianimmgr.cpp"),
            Object(NonMatching, "plugPikiNakata/panitekianimator.cpp"),
            Object(NonMatching, "plugPikiNakata/panitestsection.cpp"),
            Object(NonMatching, "plugPikiNakata/paraparameters.cpp"),
            Object(NonMatching, "plugPikiNakata/pcamcamera.cpp"),
            Object(NonMatching, "plugPikiNakata/pcamcameramanager.cpp"),
            Object(NonMatching, "plugPikiNakata/pcamcameraparameters.cpp"),
            Object(NonMatching, "plugPikiNakata/pcammotionevents.cpp"),
            Object(NonMatching, "plugPikiNakata/peve.cpp"),
            Object(NonMatching, "plugPikiNakata/peveconditions.cpp"),
            Object(NonMatching, "plugPikiNakata/pevemotionevents.cpp"),
            Object(NonMatching, "plugPikiNakata/tai.cpp"),
            Object(NonMatching, "plugPikiNakata/taiattackactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taibasicactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taichappy.cpp"),
            Object(NonMatching, "plugPikiNakata/taicollec.cpp"),
            Object(NonMatching, "plugPikiNakata/taicollisionactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taieffectactions.cpp"),
            Object(NonMatching, "plugPikiNakata/taiiwagen.cpp"),
        ],
    },
    {
        "lib": "plugPikiNishimura",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiNishimura/Pom.cpp"),
            Object(NonMatching, "plugPikiNishimura/PomAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/RumbleData.cpp"),
            Object(NonMatching, "plugPikiNishimura/Slime.cpp"),
            Object(NonMatching, "plugPikiNishimura/SlimeAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/SlimeBody.cpp"),
            Object(NonMatching, "plugPikiNishimura/SlimeCreature.cpp"),
            Object(NonMatching, "plugPikiNishimura/Snake.cpp"),
            Object(NonMatching, "plugPikiNishimura/SnakeAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/SnakeBody.cpp"),
            Object(NonMatching, "plugPikiNishimura/Spider.cpp"),
            Object(NonMatching, "plugPikiNishimura/SpiderAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/SpiderLeg.cpp"),
            Object(NonMatching, "plugPikiNishimura/Boss.cpp"),
            Object(NonMatching, "plugPikiNishimura/BossAnimMgr.cpp"),
            Object(NonMatching, "plugPikiNishimura/BossCnd.cpp"),
            Object(NonMatching, "plugPikiNishimura/BossMgr.cpp"),
            Object(NonMatching, "plugPikiNishimura/BossShapeObj.cpp"),
            Object(NonMatching, "plugPikiNishimura/CoreNucleus.cpp"),
            Object(NonMatching, "plugPikiNishimura/CoreNucleusAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/genBoss.cpp"),
            Object(NonMatching, "plugPikiNishimura/HmRumbleMgr.cpp"),
            Object(NonMatching, "plugPikiNishimura/HmRumbleSample.cpp"),
            Object(NonMatching, "plugPikiNishimura/King.cpp"),
            Object(NonMatching, "plugPikiNishimura/KingAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/KingBack.cpp"),
            Object(NonMatching, "plugPikiNishimura/KingBody.cpp"),
            Object(NonMatching, "plugPikiNishimura/Kogane.cpp"),
            Object(NonMatching, "plugPikiNishimura/KoganeAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/Mizu.cpp"),
            Object(NonMatching, "plugPikiNishimura/MizuAi.cpp"),
            Object(NonMatching, "plugPikiNishimura/nscalculation.cpp"),
            Object(NonMatching, "plugPikiNishimura/Nucleus.cpp"),
            Object(NonMatching, "plugPikiNishimura/NucleusAi.cpp"),
        ],
    },
    {
        "lib": "plugPikiOgawa",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiOgawa/ogStart.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogSub.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogTest.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogTitle.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogTotalScore.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogTutorial.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogTutorialData.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogCallBack.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogDiary.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogFileChkSel.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogFileCopy.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogFileDelete.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogFileSelect.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogGraph.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogMakeDefault.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogMap.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogMemChk.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogMenu.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogMessage.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogNitaku.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogPause.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogRader.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogResult.cpp"),
            Object(NonMatching, "plugPikiOgawa/ogSave.cpp"),
        ],
    },
    {
        "lib": "plugPikiYamashita",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "plugPikiYamashita/TAImar.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAImiurin.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIotama.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAItamago.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAItank.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIusuba.cpp"),
            Object(NonMatching, "plugPikiYamashita/tekiyamashita.cpp"),
            Object(NonMatching, "plugPikiYamashita/tekiyteki.cpp"),
            Object(NonMatching, "plugPikiYamashita/texAnim.cpp"),
            Object(NonMatching, "plugPikiYamashita/yai.cpp"),
            Object(NonMatching, "plugPikiYamashita/zenController.cpp"),
            Object(NonMatching, "plugPikiYamashita/zenGraphics.cpp"),
            Object(NonMatching, "plugPikiYamashita/zenMath.cpp"),
            Object(NonMatching, "plugPikiYamashita/zenSys.cpp"),
            Object(NonMatching, "plugPikiYamashita/alphaWipe.cpp"),
            Object(NonMatching, "plugPikiYamashita/bBoardColourAnim.cpp"),
            Object(NonMatching, "plugPikiYamashita/damageEffect.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawAccount.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCMbest.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCMcourseSelect.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCMresult.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCMscore.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCMtitle.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCommon.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawContainer.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawCountDown.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawFinalResult.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawGameInfo.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawGameOver.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawHiScore.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawHurryUp.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawMenu.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawMenuBase.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawOptionSave.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawProgre.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawSaveFailure.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawSaveMes.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawUfoParts.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawWMPause.cpp"),
            Object(NonMatching, "plugPikiYamashita/drawWorldMap.cpp"),
            Object(NonMatching, "plugPikiYamashita/effectMgr2D.cpp"),
            Object(NonMatching, "plugPikiYamashita/effectMgr.cpp"),
            Object(NonMatching, "plugPikiYamashita/gameCourseClear.cpp"),
            Object(NonMatching, "plugPikiYamashita/gameCredits.cpp"),
            Object(NonMatching, "plugPikiYamashita/gameStageClear.cpp"),
            Object(NonMatching, "plugPikiYamashita/menuPanelMgr.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DFont.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DGrafContext.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DOrthoGraph.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DPane.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DPerspGraph.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DPicture.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DPrint.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DScreen.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DStream.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DTextBox.cpp"),
            Object(NonMatching, "plugPikiYamashita/P2DWindow.cpp"),
            Object(NonMatching, "plugPikiYamashita/particleGenerator.cpp"),
            Object(NonMatching, "plugPikiYamashita/particleLoader.cpp"),
            Object(NonMatching, "plugPikiYamashita/particleManager.cpp"),
            Object(NonMatching, "plugPikiYamashita/particleMdlManager.cpp"),
            Object(NonMatching, "plugPikiYamashita/solidField.cpp"),
            Object(NonMatching, "plugPikiYamashita/PSUList.cpp"),
            Object(NonMatching, "plugPikiYamashita/ptclGenPack.cpp"),
            Object(NonMatching, "plugPikiYamashita/PUTRect.cpp"),
            Object(NonMatching, "plugPikiYamashita/simpleParticle.cpp"),
            Object(NonMatching, "plugPikiYamashita/spectrumCursorMgr.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAattack.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIanimation.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAeffect.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAjudge.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAmotion.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAmove.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIAreaction.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIbeatle.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIdororo.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIeffectAttack.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIhibaA.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIkabekuiA.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIkabekuiB.cpp"),
            Object(NonMatching, "plugPikiYamashita/TAIkabekuiC.cpp"),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": "GC/3.0a5.2",
        "progress_category": "sdk",
        "cflags": [*cflags_runtime, "-inline deferred"],
        "objects": [
            Object(NonMatching, "Runtime/PPCEABI/H/__mem.c"),
            Object(NonMatching, "Runtime/PPCEABI/H/__va_arg.c"),
            Object(NonMatching, "Runtime/PPCEABI/H/global_destructor_chain.c"),
            Object(NonMatching, "Runtime/PPCEABI/H/NMWException.cp"),
            Object(NonMatching, "Runtime/PPCEABI/H/ptmf.c"),
            Object(NonMatching, "Runtime/PPCEABI/H/runtime.c"),
            Object(NonMatching, "Runtime/PPCEABI/H/__init_cpp_exceptions.cpp"),
            Object(NonMatching, "Runtime/PPCEABI/H/Gecko_ExceptionPPC.cp"),
            Object(NonMatching, "Runtime/PPCEABI/H/GCN_mem_alloc.c"),
        ],
    },
    {
        "lib": "egg_gfx",
        "cflags": [*cflags_egg, "-Cpp_exceptions on"],
        "mw_version": "GC/3.0a3p1",  # unknown
        "progress_category": "egg",
        "objects": [
            Object(
                NonMatching,
                "egg/gfx/eggCamera.cpp",
                extra_cflags=["-RTTI on"],
            ),
            Object(NonMatching, "egg/gfx/eggDrawHelper.cpp"),
            Object(NonMatching, "egg/gfx/eggTexture.cpp"),
            Object(NonMatching, "egg/gfx/eggPalette.cpp"),
            Object(
                NonMatching,
                "egg/gfx/eggProjection.cpp",
                extra_cflags=["-RTTI on"],
            ),
            Object(NonMatching, "egg/gfx/eggViewport.cpp"),
        ],
    },
    {
        "lib": "egg_prim",
        "cflags": [*cflags_egg, "-Cpp_exceptions on"],
        "mw_version": "GC/3.0a3p1",  # unknown
        "progress_category": "egg",
        "objects": [
            Object(NonMatching, "egg/prim/eggAssert.cpp"),
            Object(NonMatching, "egg/prim/eggList.cpp"),
        ],
    },
    {
        "lib": "egg_math",
        "cflags": [*cflags_egg, "-Cpp_exceptions on"],
        "mw_version": "GC/3.0a3p1",  # unknown
        "progress_category": "egg",
        "objects": [
            Object(NonMatching, "egg/math/eggMath.cpp"),
            Object(NonMatching, "egg/math/eggMatrix.cpp"),
            Object(NonMatching, "egg/math/eggVector.cpp"),
            Object(NonMatching, "egg/math/eggBoundBox.cpp"),
        ],
    },
    {
        "lib": "egg_core",
        "cflags": cflags_egg,
        "mw_version": "GC/3.0a3p1",  # unknown
        "progress_category": "egg",
        "objects": [
            Object(NonMatching, "egg/core/eggExpHeap.cpp"),
            Object(NonMatching, "egg/core/eggHeap.cpp"),
            Object(NonMatching, "egg/core/eggAllocator.cpp"),
            Object(NonMatching, "egg/core/eggThread.cpp"),
            Object(NonMatching, "egg/core/eggSystem.cpp"),
            Object(NonMatching, "egg/core/eggController.cpp"),
            Object(NonMatching, "egg/core/eggStream.cpp"),
            Object(NonMatching, "egg/core/eggDvdRipper.cpp"),
            Object(NonMatching, "egg/core/eggDvdFile.cpp"),
            Object(NonMatching, "egg/core/eggDisposer.cpp"),
            Object(NonMatching, "egg/core/eggArchive.cpp"),
            Object(NonMatching, "egg/core/eggLongStopWatch.cpp"),
            Object(NonMatching, "egg/core/eggDecomp.cpp"),
        ],
    },
    {
        "lib": "egg_util",
        "cflags": cflags_egg,
        "mw_version": "GC/3.0a3p1",  # unknown
        "progress_category": "egg",
        "objects": [
            Object(NonMatching, "egg/util/eggSaveBanner.cpp"),
            # TODO: one extra function exists here - new file or in eggSaveBanner?
        ],
    },
    {
        "lib": "nw4r_ut",
        "cflags": cflags_nw4r,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/ut/ut_list.cpp"),
            Object(NonMatching, "nw4r/ut/ut_LinkList.cpp"),
            Object(NonMatching, "nw4r/ut/ut_binaryFileFormat.cpp"),
            Object(NonMatching, "nw4r/ut/ut_CharStrmReader.cpp"),
            Object(NonMatching, "nw4r/ut/ut_TagProcessorBase.cpp"),
            Object(NonMatching, "nw4r/ut/ut_LockedCache.cpp"),
            Object(NonMatching, "nw4r/ut/ut_Font.cpp"),
            Object(NonMatching, "nw4r/ut/ut_RomFont.cpp"),
            Object(NonMatching, "nw4r/ut/ut_ResFontBase.cpp"),
            Object(NonMatching, "nw4r/ut/ut_ResFont.cpp"),
            Object(NonMatching, "nw4r/ut/ut_CharWriter.cpp"),
            Object(NonMatching, "nw4r/ut/ut_TextWriterBase.cpp"),
        ],
    },
    {
        "lib": "nw4r_math",
        "cflags": cflags_nw4r,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/math/math_arithmetic.cpp"),
            Object(NonMatching, "nw4r/math/math_triangular.cpp"),
            Object(NonMatching, "nw4r/math/math_types.cpp"),
            Object(NonMatching, "nw4r/math/math_geometry.cpp"),
        ],
    },
    {
        "lib": "nw4r_db",
        "cflags": cflags_nw4r,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/db/db_directPrint.cpp"),
            Object(NonMatching, "nw4r/db/db_console.cpp"),
            Object(NonMatching, "nw4r/db/db_assert.cpp"),
        ],
    },
    {
        "lib": "nw4r_g3d",
        "cflags": cflags_nw4r,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/g3d/g3d_rescommon.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resdict.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resfile.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resmdl.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resshp.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_restev.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resmat.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resvtx.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_restex.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resnode.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanm.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanmvis.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanmclr.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanmtexpat.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanmtexsrt.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_resanmchr.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmvis.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmclr.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmtexpat.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmtexsrt.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmchr.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmshp.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmscn.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_obj.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_anmobj.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_gpu.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_cpu.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_state.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_draw1mat1shp.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_calcview.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_dcc.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_workmem.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_calcworld.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_draw.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_camera.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_basic.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_maya.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_xsi.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_3dsmax.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_scnobj.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_scnroot.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_scnmdlsmpl.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_scnmdl.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_calcmaterial.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_init.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_fog.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_light.cpp"),
            Object(NonMatching, "nw4r/g3d/g3d_calcvtx.cpp"),
        ],
    },
    {
        "lib": "nw4r_lyt",
        "cflags": cflags_nw4r,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "nw4r",
        "objects": [
            Object(NonMatching, "nw4r/lyt/lyt_pane.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_group.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_layout.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_picture.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_textBox.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_window.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_bounding.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_material.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_texMap.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_drawInfo.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_animation.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_resourceAccessor.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_arcResourceAccessor.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_common.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_util.cpp"),
            Object(NonMatching, "nw4r/lyt/lyt_u.cpp"),
        ],
    },
    {
        "lib": "System12",
        "cflags": cflags_base,
        "mw_version": "GC/3.0a3p1",
        "progress_category": "sys12",
        "objects": [
            Object(NonMatching, "System12/sys12System.cpp"),
            Object(NonMatching, "System12/system12Controller.cpp"),
            Object(NonMatching, "System12/sys12GameConfig.cpp"),
            Object(NonMatching, "System12/sys12GamePointer.cpp"),
            Object(NonMatching, "System12/sys12NANDMgr.cpp"),
            Object(NonMatching, "System12/sys12SDMgr.cpp"),
            Object(NonMatching, "System12/sys12TagParms.cpp"),
            Object(NonMatching, "System12/sys12gameNand.cpp"),
            Object(NonMatching, "System12/sys12Language.cpp"),
            Object(NonMatching, "System12/sys12HomeButtonMgr.cpp"),
            Object(NonMatching, "System12/sys12RenderMode.cpp"),
            Object(NonMatching, "System12/sys12TransparentMgr.cpp"),
            Object(NonMatching, "System12/sys12WarnWindow.cpp"),
            Object(NonMatching, "System12/sys12ControllerBatteryMgr.cpp"),
            Object(NonMatching, "System12/system12DvdLoader.cpp"),
            Object(NonMatching, "System12/system12FrameCounter.cpp"),
            Object(NonMatching, "System12/system12Layout.cpp"),
            # TODO: split another file in between here
            Object(NonMatching, "System12/sys12THPPlayer.cpp"),
            Object(NonMatching, "System12/sys12DebugDrawer.cpp"),
            Object(NonMatching, "System12/system12WPMessage.cpp"),
            Object(NonMatching, "System12/system12WPTagProcessor.cpp"),
            Object(NonMatching, "System12/system12WPWideTextWriter.cpp"),
            Object(NonMatching, "System12/sys12CursorMgr.cpp"),
            # TODO: split another file in between here
            Object(NonMatching, "System12/sys12DvdErrorMsg.cpp"),
            Object(NonMatching, "System12/system123D.cpp"),
            Object(NonMatching, "System12/sys12Renderer.cpp"),
            Object(NonMatching, "System12/sys12Window.cpp"),
            Object(NonMatching, "System12/system12ResourceMgr.cpp"),
            # TODO: split another 1-3 files in between here
            Object(NonMatching, "System12/sys12DpdState.cpp"),
            Object(NonMatching, "System12/eggSafeString.cpp"),
            Object(NonMatching, "System12/PSSpkSystem.cpp"),
            Object(NonMatching, "System12/SpkData.cpp"),
            Object(NonMatching, "System12/SpkGadget.cpp"),
            Object(NonMatching, "System12/SpkMixingBuffer.cpp"),
            Object(NonMatching, "System12/SpkSound.cpp"),
            Object(NonMatching, "System12/SpkSpeakerCtrl.cpp"),
            Object(NonMatching, "System12/SpkSystem.cpp"),
            Object(NonMatching, "System12/SpkTable.cpp"),
            Object(NonMatching, "System12/SpkWave.cpp"),
        ],
    },
    {
        "lib": "homebuttonLib",
        "cflags": cflags_pikmin,
        "mw_version": "GC/3.0a3",
        "progress_category": "hbm",
        "objects": [
            Object(NonMatching, "homebuttonLib/HBMFrameController.cpp"),
            Object(NonMatching, "homebuttonLib/HBMAnmController.cpp"),
            Object(NonMatching, "homebuttonLib/HBMGUIManager.cpp"),
            Object(NonMatching, "homebuttonLib/HBMController.cpp"),
            Object(NonMatching, "homebuttonLib/HBMRemoteSpk.cpp"),
            Object(NonMatching, "homebuttonLib/HBMAxSound.cpp"),
            Object(NonMatching, "homebuttonLib/HBMCommon.c"),
            Object(NonMatching, "homebuttonLib/HBMBase.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_animation.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_arcResourceAccessor.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_bounding.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_common.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_drawInfo.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_group.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_layout.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_material.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_pane.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_picture.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_resourceAccessor.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_textBox.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/lyt/lyt_window.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/math/math_triangular.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_binaryFileFormat.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_CharStrmReader.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_CharWriter.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_Font.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_LinkList.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_list.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_ResFont.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_ResFontBase.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_TagProcessorBase.cpp"),
            Object(NonMatching, "homebuttonLib/nw4hbm/ut/ut_TextWriterBase.cpp"),
            Object(NonMatching, "homebuttonLib/sound/mix.c"),
            Object(NonMatching, "homebuttonLib/sound/syn.c"),
            Object(NonMatching, "homebuttonLib/sound/synctrl.c"),
            Object(NonMatching, "homebuttonLib/sound/synenv.c"),
            Object(NonMatching, "homebuttonLib/sound/synmix.c"),
            Object(NonMatching, "homebuttonLib/sound/synpitch.c"),
            Object(NonMatching, "homebuttonLib/sound/synsample.c"),
            Object(NonMatching, "homebuttonLib/sound/synvoice.c"),
            Object(NonMatching, "homebuttonLib/sound/seq.c"),
        ],
    },
    RevolutionLib(
        "esp",
        [
            Object(NonMatching, "esp/esp.c"),
        ],
    ),
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or NonMatching) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
    ProgressCategory("jaudio", "JAudio Code"),
    ProgressCategory("nw4r", "NintendoWare Code"),
    ProgressCategory("egg", "EGG Code"),
    ProgressCategory("sys12", "System12 Code"),
    ProgressCategory("hbm", "HomeButtonMenu Code"),
    ProgressCategory("thp", "THPSimple Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
