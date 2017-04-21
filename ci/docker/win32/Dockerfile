FROM fedora
RUN dnf install -y \
    gcc \
    gcc-c++ \
    make \
    git \
    autoconf \
    automake \
    wget \
    which \
    bison \
    bzip2 \
    flex \
    gperf \
    intltool \
    libtool \
    openssl \
    patch \
    python \
    ruby \
    scons \
    unzip \
    xz \
    p7zip \
    gdk-pixbuf2 \
    gdk-pixbuf2-devel
RUN git clone https://github.com/mxe/mxe.git /mxe
RUN make -C /mxe \
        MXE_TARGETS="i686-w64-mingw32.shared" \
        qtbase qtdeclarative qtgraphicaleffects \
        qtimageformats qtlocation qtmultimedia \
        qtquickcontrols qtquickcontrols2 qtsensors \
        qtservice qtsvg qttranslations qtwinextras \
        qtxmlpatterns qttools
