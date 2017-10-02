FROM debian

RUN apt-get update && \
    apt-get install -y \
        autoconf automake autopoint bash bison bzip2 flex gettext \
        git g++ gperf intltool libffi-dev libgdk-pixbuf2.0-dev \
        libtool-bin libltdl-dev libssl-dev libxml-parser-perl make \
        openssl p7zip-full patch perl pkg-config python ruby scons \
        sed unzip wget xz-utils python3 && \
    cd /opt && \
    git clone git clone https://github.com/mxe/mxe.git && \
    cd mxe && \
    make MXE_TARGETS='x86_64-w64-mingw32.shared' \
        JOBS=16 \
        qt5
