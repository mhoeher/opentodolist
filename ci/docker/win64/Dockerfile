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
        mxe-x86-64-w64-mingw32.shared-qtbase \
        mxe-x86-64-w64-mingw32.shared-qtdeclarative \
        mxe-x86-64-w64-mingw32.shared-qtgraphicaleffects \
        mxe-x86-64-w64-mingw32.shared-qtimageformats \
        mxe-x86-64-w64-mingw32.shared-qtmultimedia \
        mxe-x86-64-w64-mingw32.shared-qtquickcontrols \
        mxe-x86-64-w64-mingw32.shared-qtquickcontrols2 \
        mxe-x86-64-w64-mingw32.shared-qtsvg \
        mxe-x86-64-w64-mingw32.shared-qttools \
        mxe-x86-64-w64-mingw32.shared-qttranslations \
        mxe-x86-64-w64-mingw32.shared-qtwinextras \
        mxe-x86-64-w64-mingw32.shared-qtxmlpatterns
