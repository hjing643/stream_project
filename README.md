# stream_project
yuv,h264,rgp

compile ffmepg

1. x264
cd ~/third-party/ffmpeg6.0
git clone --branch stable https://code.videolan.org/videolan/x264.git
cd x264
./configure --prefix=$HOME/third-party/ffmpeg6.0/linux_build --enable-static --disable-opencl
make -j$(nproc)
make install

2. x265
cd ~/third-party/ffmpeg6.0
git clone https://bitbucket.org/multicoreware/x265_git.git
cd x265_git/build/linux
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$HOME/third-party/ffmpeg6.0/linux_build -DENABLE_SHARED=OFF ../../source
make -j$(nproc)
make install

3.libvpx
cd ~/third-party/ffmpeg6.0
git clone https://chromium.googlesource.com/webm/libvpx
cd libvpx
./configure --prefix=$HOME/third-party/ffmpeg6.0/linux_build --disable-examples --disable-unit-tests --enable-vp8 --enable-vp9 --enable-static --disable-shared
make -j$(nproc)
make install


4. fdk-aac, libpng, libjpeg, OpenCL, CUDA/NvCodec，libsrtp, libheif, libde265


5. ffmpeg(only master version support libheif)
PKG_CONFIG_PATH="$HOME/third-party/ffmpeg6.0/linux_build/lib/pkgconfig"

./configure \
  --prefix="$HOME/third-party/ffmpeg6.0/linux_build" \
  --pkg-config-flags="--static" \
  --extra-cflags="-I$HOME/third-party/ffmpeg6.0/linux_build/include" \
  --extra-ldflags="-L$HOME/third-party/ffmpeg6.0/linux_build/lib" \
  --extra-libs="-lpthread -lm" \
  --bindir="$HOME/third-party/ffmpeg6.0/linux_build/bin" \
  --enable-gpl \
  --enable-nonfree \
  --enable-libx264 \
  --enable-libx265 \
  --enable-libvpx \
  --enable-libfdk-aac \
  --enable-static \
  --disable-shared \
  --disable-debug \
  --disable-doc \
  --disable-asm \
  --enable-libheif