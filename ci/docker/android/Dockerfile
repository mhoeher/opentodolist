# Minimal docker container to build project

FROM ubuntu:18.04
#MAINTAINER Rabit <home@rabits.org> (@rabits)

ARG QT_VERSION=5.15.0
ARG QT_INSTALLER_URL="http://master.qt.io/archive/online_installers/3.2/qt-unified-linux-x64-3.2.1-2-online.run"
ARG QT_INSTALLER_SHA256="02a3445e5b8dab761946ad6f6f3d80ccf9a3246d689bcbec69112379dd523506"
# Use to pass URL with env exports file context with QT_CI_LOGIN & QT_CI_PASSWORD
ARG QT_CI_ENV_URL=""

ARG NDK_VERSION=21.0.6113669

ARG SDK_URL="https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip"
ARG SDK_SHA256="92ffee5a1d98d856634e8b71132e8a95d96c83a63fde1099be3d86df3106def9"
ARG ANDROID_NATIVE_API_LEVEL=29
ARG SDK_PLATFORM=android-${ANDROID_NATIVE_API_LEVEL}
ARG SDK_INSTALL_DIR=/opt/android-sdk
ARG SDK_BUILD_TOOLS=${ANDROID_NATIVE_API_LEVEL}.0.3
ARG SDK_PACKAGES="tools platform-tools"
ARG QT_CI_LOGIN
ARG QT_CI_PASSWORD

ENV DEBIAN_FRONTEND=noninteractive \
    QT_PATH=/opt/Qt \
    QT_BIN_PACKAGE=android
ENV QT_ANDROID=${QT_PATH}/${QT_VERSION}/${QT_BIN_PACKAGE} \
    ANDROID_HOME=${SDK_INSTALL_DIR} \
    ANDROID_SDK_ROOT=${SDK_INSTALL_DIR} \
    ANDROID_SDK_BUILD_TOOLS=${SDK_BUILD_TOOLS} \
    ANDROID_NDK_ROOT=/opt/android-sdk/ndk/${NDK_VERSION} \
    ANDROID_NDK_HOST=linux-x86_64 \
    ANDROID_NDK_PLATFORM=${SDK_PLATFORM} \
    ANDROID_NATIVE_API_LEVEL=${ANDROID_NATIVE_API_LEVEL} \
    QMAKESPEC=android-g++
ENV PATH ${QT_PATH}/Tools/CMake/bin:${QT_PATH}/Tools/Ninja:${ANDROID_HOME}/tools:${ANDROID_HOME}/tools/bin:${ANDROID_HOME}/platform-tools:${PATH}

# Install updates & requirements:
#  * unzip - unpack platform tools
#  * git, openssh-client, ca-certificates - clone & build
#  * locales, sudo - useful to set utf-8 locale & sudo usage
#  * curl - to download Qt bundle
#  * make, openjdk-8-jdk-headless, ant - basic build requirements
#  * libsm6, libice6, libxext6, libxrender1, libfontconfig1, libdbus-1-3, libx11-xcb1 - dependencies of Qt bundle run-file
#  * libc6:i386, libncurses5:i386, libstdc++6:i386, libz1:i386 - dependencides of android sdk binaries
RUN dpkg --add-architecture i386 && apt update && apt full-upgrade -y && apt install -y --no-install-recommends \
    unzip \
    git \
    openssh-client \
    ca-certificates \
    locales \
    curl \
    make \
    openjdk-8-jdk-headless \
    openjdk-8-jre-headless \
    ant \
    ccache \
    libsm6 \
    libice6 \
    libxext6 \
    libxrender1 \
    libfontconfig1 \
    libdbus-1-3 \
    libx11-xcb1 \
    libxkbcommon-x11-0 \
    libc6:i386 \
    libncurses5:i386 \
    libstdc++6:i386 \
    libz1:i386 \
    python \
    && apt-get -qq clean \
    && rm -rf /var/lib/apt/lists/*

COPY extract-qt-installer.sh /tmp/qt/

# Download & unpack Qt toolchains & clean
RUN echo "${QT_INSTALLER_SHA256} -" > sum.txt && curl -fLs "${QT_INSTALLER_URL}" | tee /tmp/qt/installer.run | sha256sum -c sum.txt \
    && [ -z "${QT_CI_ENV_URL}" ] && echo "" > /tmp/qt/secrets.env || curl "${QT_CI_ENV_URL}" > /tmp/qt/secrets.env && . /tmp/qt/secrets.env \
    && export QT_CI_LOGIN="${QT_CI_LOGIN}" && export QT_CI_PASSWORD="${QT_CI_PASSWORD}" \
    && QT_CI_PACKAGES=qt.qt5.$(echo "${QT_VERSION}" | tr -d .).${QT_BIN_PACKAGE},qt.tools.cmake,qt.tools.ninja /tmp/qt/extract-qt-installer.sh /tmp/qt/installer.run "${QT_PATH}" \
    && find "${QT_PATH}" -mindepth 1 -maxdepth 1 ! -name "${QT_VERSION}" ! -name "Tools" -exec echo 'Cleaning Qt SDK: {}' \; -exec rm -r '{}' \; \
    && rm -rf "${QT_PATH}/Tools/QtCreator" /tmp/qt /root/.local/share/Qt /root/.config/Qt

# Download & unpack android SDK
RUN echo "${SDK_SHA256} -" > sum.txt && curl -fLs "${SDK_URL}" | tee /tmp/sdk-tools.zip | sha256sum -c sum.txt \
    && mkdir -p /opt/android-sdk && unzip -q /tmp/sdk-tools.zip -d /opt/android-sdk && rm -f /tmp/sdk-tools.zip \
    && yes | sdkmanager --licenses && sdkmanager --update && sdkmanager --verbose "platforms;${SDK_PLATFORM}" "ndk;${NDK_VERSION}" "build-tools;${SDK_BUILD_TOOLS}" ${SDK_PACKAGES}

# Reconfigure locale
RUN locale-gen en_US.UTF-8 && dpkg-reconfigure locales

ENV QT_ROOT /opt/Qt/5.15.0/android/
