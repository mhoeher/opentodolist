FROM ubuntu:xenial

RUN \
    apt-get update && \
    apt-get install -y \
        software-properties-common \
        wget \
        curl && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    add-apt-repository -y ppa:beineri/opt-qt-5.14.1-xenial && \
    apt-get update && \
    apt-get install -y \
        qt514base \
        qt514declarative \
        qt514imageformats \
        qt514quickcontrols2 \
        qt514svg \
        qt514translations \
        qt514wayland \
        qt514x11extras \
        qt514tools \
        build-essential \
        libfontconfig1 \
        desktop-file-utils \
        libwayland-dev \
        libwayland-egl1-mesa \
        libwayland-server0 \
        libgles2-mesa-dev \
        libxkbcommon-dev \
        libgl1-mesa-dev \
        libfuse2 \
        libxcomposite1 \
        git \
        cmake \
        libssl-dev \
        ninja-build \
        libxrender1 \
        libxkbcommon-x11-0 \
        libodbc1 \
        libpq5 \
    && \
    apt-get clean

ENV QT_ROOT /opt/qt514
