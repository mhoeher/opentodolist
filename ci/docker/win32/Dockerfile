FROM fedora
RUN dnf install -y \
    gcc \
    gcc-c++ \
    make \
    git
RUN git clone https://github.com/mxe/mxe.git /mxe
RUN make -C /mxe \
        MXE_TARGETS="i686-w64-mingw32.shared" \
        qtbase qtdeclarative qtgraphicaleffects \
        qtimageformats qtlocation qtmultimedia \
        qtquickcontrols qtquickcontrols2 qtsensors \
        qtservice qtsvg qttranslations qtwinextras \
        qtxmlpatterns qttools
