FROM debian


# Enable MXE pre-build repository:
RUN echo "deb http://pkg.mxe.cc/repos/apt/debian wheezy main" > \
    /etc/apt/sources.list.d/mxeapt.list && \
    \
    apt-get update && \
    apt-get install -y gnupg2 && \
    \
    apt-key adv --keyserver keyserver.ubuntu.com \
        --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB && \
    \
    apt-get update && \
    apt-get install -y \
        git \
        mxe-i686-w64-mingw32.shared-qtbase \
        mxe-i686-w64-mingw32.shared-qtdeclarative \
        mxe-i686-w64-mingw32.shared-qtgraphicaleffects \
        mxe-i686-w64-mingw32.shared-qtimageformats \
        mxe-i686-w64-mingw32.shared-qtmultimedia \
        mxe-i686-w64-mingw32.shared-qtquickcontrols \
        mxe-i686-w64-mingw32.shared-qtquickcontrols2 \
        mxe-i686-w64-mingw32.shared-qtsvg \
        mxe-i686-w64-mingw32.shared-qttools \
        mxe-i686-w64-mingw32.shared-qttranslations \
        mxe-i686-w64-mingw32.shared-qtwinextras \
        mxe-i686-w64-mingw32.shared-qtxmlpatterns
