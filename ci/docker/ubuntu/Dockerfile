FROM ubuntu:xenial
RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    add-apt-repository --yes ppa:beineri/opt-qt-5.10.0-xenial && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        qt510base \
        qt510declarative \
        qt510tools \
        qt510svg \
        qt510graphicaleffects \
        qt510imageformats \
        qt510translations \
        qt510quickcontrols \
        qt510quickcontrols2 \
        qt510x11extras \
        qt510websockets \
        qt510webchannel \
        qt510webengine \
        qt510remoteobjects \
        qt510wayland \
        build-essential \
        libgl1-mesa-dev \
        libfuse2 \
        desktop-file-utils \
        git && \
    apt-get clean
