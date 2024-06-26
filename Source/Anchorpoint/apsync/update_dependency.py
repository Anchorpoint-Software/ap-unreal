import os
import shutil
import subprocess
import sys

# Read APSYNC_DIR
apsync_dir = os.getenv("APSYNC_DIR")
if not apsync_dir:
    print("APSYNC_DIR is not set")
    sys.exit(1)

build_dir = os.path.join(apsync_dir, "build_ue")
print(f"build_dir: {build_dir}")

# Delete build_dir if it exists
if os.path.exists(build_dir):
    shutil.rmtree(build_dir)

# Create build_dir
os.makedirs(build_dir)

# Change to the script's directory
script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

# Change to the build_dir
os.chdir(build_dir)

# Run cmake and build
subprocess.run(
    [
        "cmake",
        apsync_dir,
        "-G",
        "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_CXX_FLAGS_RELEASE=-O3 -g",
        "-DAPSYNC_STATIC=OFF",
        "-DBUILD_PYTHON_MODULE=OFF",
        "-DBUILD_UNIVERSAL_LIB=ON",
    ],
    check=True,
)

subprocess.run(["cmake", "--build", ".", "--config", "Release"], check=True)

if sys.platform == "darwin":
    # macOS specific commands
    subprocess.run(
        ["strip", "-x", os.path.join(build_dir, "libsync.dylib")], check=True
    )
    subprocess.run(
        [
            "codesign",
            "--deep",
            "--force",
            "--verify",
            "--verbose",
            "--timestamp",
            "--sign",
            "1F84E32368E339819F9BC284B344F014694A1453",
            "--options",
            "runtime",
            os.path.join(build_dir, "libsync.dylib"),
        ],
        check=True,
    )

# Copy headers and libraries
include_dir = os.path.join(script_dir, "include")
if os.path.exists(include_dir):
    shutil.rmtree(include_dir)

shutil.copytree(os.path.join(apsync_dir, "include"), include_dir)
shutil.copy(
    os.path.join(apsync_dir, "extern", "sole", "sole.hpp"),
    os.path.join(include_dir, "sole.hpp"),
)
shutil.copy(
    os.path.join(apsync_dir, "extern", "sole", "sole.cpp"),
    os.path.join(include_dir, "sole.cpp"),
)

if sys.platform == "darwin":
    mac_dir = os.path.join(script_dir, "mac")
    if not os.path.exists(mac_dir):
        os.makedirs(mac_dir)
    shutil.copy(
        os.path.join(build_dir, "libsync.dylib"), os.path.join(mac_dir, "libsync.dylib")
    )
elif sys.platform == "windows":
    win_dir = os.path.join(script_dir, "win")
    if not os.path.exists(win_dir):
        os.makedirs(win_dir)
    shutil.copy(os.path.join(build_dir, "sync.dll"), os.path.join(win_dir, "sync.dll"))
    shutil.copy(os.path.join(build_dir, "sync.lib"), os.path.join(win_dir, "sync.lib"))

print("Build completed successfully.")