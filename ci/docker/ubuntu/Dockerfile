FROM ubuntu:xenial
RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    add-apt-repository --yes ppa:beineri/opt-qt-5.12.3-xenial && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        qt512base \
        qt512declarative \
        qt512tools \
        qt512svg \
        qt512graphicaleffects \
        qt512imageformats \
        qt512translations \
        qt512quickcontrols \
        qt512quickcontrols2 \
        qt512x11extras \
        qt512websockets \
        qt512webchannel \
        qt512webengine \
        qt512remoteobjects \
        qt512wayland \
        build-essential \
        libgl1-mesa-dev \
        libfuse2 \
        desktop-file-utils \
        git \
        cmake \
        libssl-dev \
        ninja-build \
        && \
    apt-get clean
