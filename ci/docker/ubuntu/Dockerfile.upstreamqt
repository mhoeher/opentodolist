FROM ubuntu:xenial

ADD qt-installer-script.js /

RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        build-essential \
        libfontconfig1 \
        desktop-file-utils \
        libwayland-dev \
        libwayland-egl1-mesa \
        libwayland-server0 \
        libgles2-mesa-dev \
        libxkbcommon-dev \
        git && \
    wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run && \
    chmod +x qt-unified-linux-x64-online.run && \
    ./qt-unified-linux-x64-online.run --script qt-installer-script.js \
        --platform minimal --no-force-installations -v && \
    rm qt-unified-linux-x64-online.run && \
    apt-get clean
