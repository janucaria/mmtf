FROM ubuntu:18.04 AS development

LABEL maintainer="Januar Andaria <janucaria@gmail.com>"

WORKDIR /tmp

RUN apt-get update \
  && apt-get -y install --no-install-recommends apt-utils dialog 2>&1 \
  && apt-get install -y git \
  && apt-get install -y libssl-dev \
  && apt-get install -y make \
  && apt-get install -y pkg-config \
  && apt-get install -y software-properties-common \
  && apt-get install -y wget \
  #
  && wget -O- https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add - \
  && add-apt-repository "deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-9 main" \
  && apt-get update \
  # LLVM
  && apt-get install -y libllvm-9-ocaml-dev libllvm9 llvm-9 llvm-9-dev llvm-9-doc llvm-9-examples llvm-9-runtime \
  # Clang and co
  && apt-get install -y clang-9 clang-tools-9 clang-9-doc libclang-common-9-dev libclang-9-dev libclang1-9 clang-format-9 python-clang-9 clangd-9 \
  # libfuzzer
  && apt-get install -y libfuzzer-9-dev \
  # lldb
  && apt-get install -y lldb-9 \
  # lld (linker)
  && apt-get install -y lld-9 \
  # libc++
  && apt-get install -y libc++-9-dev libc++abi-9-dev \
  # OpenMP
  && apt-get install -y libomp-9-dev \
  # update alternative
  && update-alternatives \
  --install /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-9 90 \
  --slave   /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-9 \
  --slave   /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-9 \
  --slave   /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-9 \
  --slave   /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-9 \
  --slave   /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-9 \
  --slave   /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-9 \
  --slave   /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-9 \
  --slave   /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-9 \
  --slave   /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-9 \
  --slave   /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-9 \
  --slave   /usr/bin/llvm-mcmarkup     llvm-mcmarkup    /usr/bin/llvm-mcmarkup-9 \
  --slave   /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-9 \
  --slave   /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-9 \
  --slave   /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-9 \
  --slave   /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-9 \
  --slave   /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-9 \
  --slave   /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-9 \
  --slave   /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-9 \
  --slave   /usr/bin/llvm-strip        llvm-strip      /usr/bin/llvm-strip-9 \
  --slave   /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-9 \
  --slave   /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-9 \
  && update-alternatives --config llvm-config \
  && update-alternatives \
  --install /usr/bin/clang                 clang                 /usr/bin/clang-9 90 \
  --slave   /usr/bin/clang++               clang++               /usr/bin/clang++-9  \
  --slave   /usr/bin/asan_symbolize        asan_symbolize        /usr/bin/asan_symbolize-9 \
  --slave   /usr/bin/c-index-test          c-index-test          /usr/bin/c-index-test-9 \
  --slave   /usr/bin/clang-check           clang-check           /usr/bin/clang-check-9 \
  --slave   /usr/bin/clang-cl              clang-cl              /usr/bin/clang-cl-9 \
  --slave   /usr/bin/clang-cpp             clang-cpp             /usr/bin/clang-cpp-9 \
  --slave   /usr/bin/clang-format          clang-format          /usr/bin/clang-format-9 \
  --slave   /usr/bin/clang-format-diff     clang-format-diff     /usr/bin/clang-format-diff-9 \
  --slave   /usr/bin/clang-import-test     clang-import-test     /usr/bin/clang-import-test-9 \
  --slave   /usr/bin/clang-include-fixer   clang-include-fixer   /usr/bin/clang-include-fixer-9 \
  --slave   /usr/bin/clang-offload-bundler clang-offload-bundler /usr/bin/clang-offload-bundler-9 \
  --slave   /usr/bin/clang-query           clang-query           /usr/bin/clang-query-9 \
  --slave   /usr/bin/clang-rename          clang-rename          /usr/bin/clang-rename-9 \
  --slave   /usr/bin/clang-reorder-fields  clang-reorder-fields  /usr/bin/clang-reorder-fields-9 \
  --slave   /usr/bin/clang-tidy            clang-tidy            /usr/bin/clang-tidy-9 \
  --slave   /usr/bin/lldb                  lldb                  /usr/bin/lldb-9 \
  --slave   /usr/bin/lldb-server           lldb-server           /usr/bin/lldb-server-9 \
  && update-alternatives --config clang \
  #
  && add-apt-repository ppa:ubuntu-toolchain-r/test \
  && apt-get update \
  && apt-get install -y g++-9 \
  && update-alternatives \
  --install /usr/bin/gcc gcc /usr/bin/gcc-9 90 \
  --slave /usr/bin/g++ g++ /usr/bin/g++-9 \
  && update-alternatives --config gcc \
  #
  && true

RUN git clone -b v3.16.0 --recursive https://github.com/Kitware/CMake.git \
  && cd CMake \
  && ./bootstrap \
  && make -j$(nproc) \
  && make install \
  && cd .. \
  && rm -rf CMake

RUN git clone -b boost-1.71.0 --recursive https://github.com/boostorg/boost.git \
  && cd boost \
  && ./bootstrap.sh --without-libraries=atomic,filesystem,iostreams,python,regex,thread \
  && ./b2 variant=release install \
  && cd .. \
  && rm -rf boost

RUN git clone -b v2.11.0 https://github.com/catchorg/Catch2.git \
  && cd Catch2 \
  && mkdir build \
  && cmake \
  -H. \
  -B./build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF \
  && cmake --build ./build --config Release --target all -- -j$(getconf _NPROCESSORS_ONLN) \
  && cmake --build ./build --config Release --target install \
  && cd .. \
  && rm -rf Catch2

RUN git clone -b v2.0.0 https://github.com/microsoft/GSL.git \
  && cd GSL \
  && mkdir build \
  && cmake -H. -B./build \
  -DCMAKE_BUILD_TYPE=Release \
  -DGSL_TEST=OFF \
  -DGSL_CXX_STANDARD=17 \
  && cmake --build ./build --config Release --target all -- -j$(nproc) \
  && cmake --build ./build --config Release --target install -- -j$(nproc) \
  && cd .. \
  && rm -rf GSL

WORKDIR /app
