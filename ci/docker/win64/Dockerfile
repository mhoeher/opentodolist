FROM debian

# Note: If build fails, check out this issue:
#       https://github.com/mxe/mxe/issues/2011

RUN \
\
    apt-get update && \
    apt-get install -y \
        autoconf automake autopoint bash bison bzip2 flex gettext   \
        git g++ gperf intltool libffi-dev libgdk-pixbuf2.0-dev      \
        libtool-bin libltdl-dev libssl-dev libxml-parser-perl make  \
        openssl p7zip-full patch perl pkg-config python ruby scons  \
        sed unzip wget xz-utils nsis python3 &&                     \
\
    cd opt && \
    mkdir mxe-x64-shared && \
    cd mxe-x64-shared && \
    git clone https://github.com/mxe/mxe.git . && \
    make MXE_TARGETS='x86_64-w64-mingw32.shared' \
        JOBS=4 \
        qt5
