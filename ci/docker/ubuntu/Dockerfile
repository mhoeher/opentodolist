FROM ubuntu:bionic

# Minimal docker container to build project
# Image: rabits/qt:5.15-desktop

#MAINTAINER Rabit <home@rabits.org> (@rabits)

ARG QT_VERSION=5.15.2
ARG QT_INSTALLER_URL="http://master.qt.io/archive/online_installers/3.2/qt-unified-linux-x64-3.2.1-2-online.run"
ARG QT_INSTALLER_SHA256="02a3445e5b8dab761946ad6f6f3d80ccf9a3246d689bcbec69112379dd523506"
# Use to pass URL with env exports file context with QT_CI_LOGIN & QT_CI_PASSWORD
ARG QT_CI_ENV_URL=""
ARG QT_CI_LOGIN
ARG QT_CI_PASSWORD

ENV DEBIAN_FRONTEND=noninteractive \
    QT_PATH=/opt/Qt \
    QT_BIN_PACKAGE=gcc_64
ENV QT_DESKTOP ${QT_PATH}/${QT_VERSION}/${QT_BIN_PACKAGE}
ENV PATH ${QT_DESKTOP}/bin:${QT_PATH}/Tools/CMake/bin:${QT_PATH}/Tools/Ninja:$PATH

# Install updates & requirements:
#  * git, openssh-client, ca-certificates - clone & build
#  * locales, sudo - useful to set utf-8 locale & sudo usage
#  * curl - to download Qt bundle
#  * build-essential, pkg-config, libgl1-mesa-dev - basic Qt build requirements
#  * libsm6, libice6, libxext6, libxrender1, libfontconfig1, libdbus-1-3 - dependencies of the Qt bundle run-file
RUN apt update && apt full-upgrade -y && apt install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    ccache \
    cmake \
    cmake \
    curl \
    desktop-file-utils \
    file \
    git \
    libdbus-1-3 \
    libfontconfig1 \
    libfuse2 \
    libgl1-mesa-dev \
    libgles2-mesa-dev \
    libice6 \
    libodbc1 \
    libpq5 \
    libsecret-1-dev \
    libsm6 \
    libssl-dev \
    libssl-dev \
    libwayland-dev \
    libwayland-egl1-mesa \
    libwayland-server0 \
    libxcb-icccm4 \
    libxcb-image0 \
    libxcb-keysyms1 \
    libxcb-render-util0 \
    libxcb-xinerama0 \
    libxcomposite1 \
    libxext6 \
    libxkbcommon-dev \
    libxkbcommon-x11-0 \
    libxkbcommon-x11-0 \
    libxrender1 \
    locales \
    ninja-build \
    ninja-build \
    openssh-client \
    pkg-config \
    sudo \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/*

COPY extract-qt-installer.sh /tmp/qt/

# Download & unpack Qt toolchains & clean
RUN echo "${QT_INSTALLER_SHA256} -" > sum.txt && curl -fLs "${QT_INSTALLER_URL}" | tee /tmp/qt/installer.run | sha256sum -c sum.txt \
    && [ -z "${QT_CI_ENV_URL}" ] && echo "" > /tmp/qt/secrets.env || curl "${QT_CI_ENV_URL}" > /tmp/qt/secrets.env && . /tmp/qt/secrets.env \
    && export QT_CI_LOGIN="${QT_CI_LOGIN}" && export QT_CI_PASSWORD="${QT_CI_PASSWORD}" \
    && QT_CI_PACKAGES=qt.qt5.$(echo "${QT_VERSION}" | tr -d .).${QT_BIN_PACKAGE},qt.qt5.$(echo "${QT_VERSION}" | tr -d .).qtnetworkauth.${QT_BIN_PACKAGE},qt.tools.cmake,qt.tools.ninja /tmp/qt/extract-qt-installer.sh /tmp/qt/installer.run "${QT_PATH}" \
    && find "${QT_PATH}" -mindepth 1 -maxdepth 1 ! -name "${QT_VERSION}" ! -name "Tools" -exec echo 'Cleaning Qt SDK: {}' \; -exec rm -r '{}' \; \
    && rm -rf "${QT_PATH}/Tools/QtCreator" /tmp/qt /root/.local/share/Qt /root/.config/Qt

# Reconfigure locale
RUN locale-gen en_US.UTF-8 && dpkg-reconfigure locales

ENV QT_ROOT ${QT_DESKTOP}
