FROM ubuntu:xenial
RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    add-apt-repository --yes ppa:beineri/opt-qt-5.11.2-xenial && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        qt511base \
        qt511declarative \
        qt511tools \
        qt511svg \
        qt511graphicaleffects \
        qt511imageformats \
        qt511translations \
        qt511quickcontrols \
        qt511quickcontrols2 \
        qt511x11extras \
        qt511websockets \
        qt511webchannel \
        qt511webengine \
        qt511remoteobjects \
        qt511wayland \
        build-essential \
        libgl1-mesa-dev \
        libfuse2 \
        desktop-file-utils \
        git \
        cmake \
        libssl-dev \
        && \
    apt-get clean
