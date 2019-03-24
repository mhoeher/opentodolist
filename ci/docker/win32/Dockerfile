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
        mingw32-qt5-qmake \
        mingw32-qt5-qmldevtools \
        mingw32-qt5-qmldevtools-devel \
        mingw32-qt5-qt3d \
        mingw32-qt5-qt3d-tools \
        mingw32-qt5-qtactiveqt \
        mingw32-qt5-qtbase \
        mingw32-qt5-qtbase-devel \
        mingw32-qt5-qtcharts \
        mingw32-qt5-qtdeclarative \
        mingw32-qt5-qtgraphicaleffects \
        mingw32-qt5-qtimageformats \
        mingw32-qt5-qtlocation \
        mingw32-qt5-qtmultimedia \
        mingw32-qt5-qtquickcontrols \
        mingw32-qt5-qtsensors \
        mingw32-qt5-qtserialport \
        mingw32-qt5-qtsvg \
        mingw32-qt5-qttools \
        mingw32-qt5-qttools-tools \
        mingw32-qt5-qttranslations \
        mingw32-qt5-qtwebkit \
        mingw32-qt5-qtwebsockets \
        mingw32-qt5-qtwinextras \
        mingw32-qt5-qtxmlpatterns \
        mingw32-nsis \
        mingw32-nsiswrapper \
        \
        && \
        cd / && \
        git clone https://github.com/qt/qtquickcontrols2.git && \
        cd qtquickcontrols2 && \
        git checkout v$(mingw32-qmake-qt5 -query QT_VERSION) && \
        mingw32-qmake-qt5 && \
        make -j4 && \
        make install && \
        cd .. && \
        rm -rf qtquickcontrols2
