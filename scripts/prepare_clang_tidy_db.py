#!/usr/bin/env python3
"""Filters a Zephyr-generated compile_commands.json so clang-tidy (which uses clang's own
driver/frontend, unlike clangd) can actually parse the translation units.

Two problems, neither fixable by clang-tidy flags alone (--extra-arg only appends, never removes
or ignores):

1. A few GCC-only flags (e.g. -fno-reorder-functions, -mfp16-format=ieee) are unknown to clang's
   driver and are hard "unknown argument" errors, not warnings.
2. Clang doesn't reproduce the Zephyr SDK GCC's multilib selection, so it can't find that GCC's
   own bundled libstdc++ headers (e.g. cstdint) from --sysroot alone.

For (2), rather than hardcoding one of this toolchain's ~60 multilib header paths (fragile - the
right one depends on -mcpu/-mthumb/-mfloat-abi/etc, exactly like GCC's own multilib matching), this
queries the *real* compiler recorded in each command for its resolved system include search paths
(`-E -v`) and injects those as -isystem - the same approach clangd's query-driver support uses.
"""
import json
import os
import shlex
import subprocess
import sys
from pathlib import Path

UNSUPPORTED_FLAGS = {"-fno-reorder-functions"}
UNSUPPORTED_FLAG_PREFIXES = ("-mfp16-format=",)

# Flags that actually influence a GCC cross-compiler's multilib/header selection - enough to
# reproduce its include search path without re-running the query for every one of the thousands
# of -D/-I flags in a real command.
TARGET_FLAG_PREFIXES = ("-m", "-std", "--sysroot", "--target", "-B", "-specs")


def is_unsupported(arg):
    return arg in UNSUPPORTED_FLAGS or arg.startswith(UNSUPPORTED_FLAG_PREFIXES)


def query_system_includes(compiler, target_flags):
    proc = subprocess.run(
        [compiler, *target_flags, "-E", "-v", "-x", "c++", "-"],
        input="",
        capture_output=True,
        text=True,
        check=False,
    )

    paths = []
    in_list = False
    for line in proc.stderr.splitlines():
        if line.startswith("#include <...> search starts here:"):
            in_list = True
            continue
        if line.startswith("End of search list"):
            break
        if in_list:
            paths.append(line.strip().split(" ")[0])

    # Excludes GCC's own compiler-builtin header dir (.../lib/gcc/<target>/<ver>/include[-fixed]):
    # it ships GCC-specific versions of headers like arm_acle.h that assume GCC builtins clang
    # doesn't implement the same way. Clang already has its own compatible versions of those in
    # its resource-dir (always searched first); only the libstdc++/libc dirs are worth adding.
    return [p for p in paths if "/lib/gcc/" not in (os.path.normpath(p) + os.sep)]


def main():
    src_db_path = Path(sys.argv[1])
    dst_db_path = Path(sys.argv[2])

    entries = json.loads(src_db_path.read_text())
    include_cache = {}
    out_entries = []

    for entry in entries:
        args = shlex.split(entry["command"])
        compiler, rest = args[0], args[1:]

        target_flags = tuple(a for a in rest if a.startswith(TARGET_FLAG_PREFIXES))
        cache_key = (compiler, target_flags)
        if cache_key not in include_cache:
            include_cache[cache_key] = query_system_includes(compiler, list(target_flags))

        isystem_args = []
        for path in include_cache[cache_key]:
            isystem_args += ["-isystem", path]

        new_args = [compiler, *isystem_args, *(a for a in rest if not is_unsupported(a))]
        entry["command"] = shlex.join(new_args)
        out_entries.append(entry)

    dst_db_path.parent.mkdir(parents=True, exist_ok=True)
    dst_db_path.write_text(json.dumps(out_entries))


if __name__ == "__main__":
    main()
