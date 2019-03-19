FROM fedora:rawhide

RUN \
\
    dnf install -y --nogpgcheck \
        cmake \
        make \
        ninja-build \
        python3 \
        python \
        git \
        wget \
        \
        qt5-qtxmlpatterns-devel \
        qt5-linguist \
        appstream \
        \
        mingw64-qt5-qmake \
        mingw64-qt5-qmldevtools \
        mingw64-qt5-qmldevtools-devel \
        mingw64-qt5-qt3d \
        mingw64-qt5-qt3d-tools \
        mingw64-qt5-qtactiveqt \
        mingw64-qt5-qtbase \
        mingw64-qt5-qtbase-devel \
        mingw64-qt5-qtcharts \
        mingw64-qt5-qtdeclarative \
        mingw64-qt5-qtgraphicaleffects \
        mingw64-qt5-qtimageformats \
        mingw64-qt5-qtlocation \
        mingw64-qt5-qtmultimedia \
        mingw64-qt5-qtquickcontrols \
        mingw64-qt5-qtsensors \
        mingw64-qt5-qtserialport \
        mingw64-qt5-qtsvg \
        mingw64-qt5-qttools \
        mingw64-qt5-qttools-tools \
        mingw64-qt5-qttranslations \
        mingw64-qt5-qtwebkit \
        mingw64-qt5-qtwebsockets \
        mingw64-qt5-qtwinextras \
        mingw64-qt5-qtxmlpatterns \
        mingw32-nsis \
        mingw32-nsiswrapper \
        \
        && \
        cd / && \
        git clone https://github.com/qt/qtquickcontrols2.git && \
        cd qtquickcontrols2 && \
        git checkout v$(mingw64-qmake-qt5 -query QT_VERSION) && \
        mingw64-qmake-qt5 && \
        make -j4 && \
        make install && \
        cd .. && \
        rm -rf qtquickcontrols2
