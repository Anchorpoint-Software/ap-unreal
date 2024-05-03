# Read APSYNC_DIR
if [ -z "$APSYNC_DIR" ]; then
    echo "APSYNC_DIR is not set"
    exit 1
fi

apsync_dir="$APSYNC_DIR"
build_dir=$apsync_dir/build_ue

echo "build_dir: $build_dir"

# Delete build_dir if exists
if [ -d "$build_dir" ]; then
    rm -rf $build_dir
fi

pushd $(dirname $0)

mkdir $build_dir
pushd $build_dir

cmake $apsync_dir -G "Ninja" -DCMAKE_BUILD_TYPE="Release" -DCMAKE_CXX_FLAGS_RELEASE="-O3 -g" -DAPSYNC_STATIC=OFF -DBUILD_PYTHON_MODULE=OFF -DBUILD_UNIVERSAL_LIB=ON
cmake --build . --config Release

strip -x $build_dir/libsync.dylib

codesign --deep --force --verify --verbose --timestamp --sign "1F84E32368E339819F9BC284B344F014694A1453" --options runtime $build_dir/libsync.dylib

popd

rm -rf include
cp -r $apsync_dir/include include
cp $apsync_dir/extern/sole/sole.hpp include/sole.hpp
cp $apsync_dir/extern/sole/sole.cpp include/sole.cpp
cp $build_dir/libsync.dylib mac/libsync.dylib

popd
