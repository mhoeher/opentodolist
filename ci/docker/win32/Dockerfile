FROM debian


# Enable MXE pre-build repository:
RUN echo "deb http://pkg.mxe.cc/repos/apt/debian wheezy main" > \
    /etc/apt/sources.list.d/mxeapt.list
RUN apt-key adv --keyserver keyserver.ubuntu.com \
    --recv-keys D43A795B73B16ABE9643FE1AFD8FFF16DB45C6AB


# Install required packages:
RUN apt-get update
RUN apt-get install -y \
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


# Add a custom Qt configuration, so qmake will link
# against the shared version of the libraries:
ADD qt.conf /usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5/bin


# Copy the qtmain library to the bin directory, so
# the build process finds it:
RUN cp /usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5/lib/libqtmain.a \
    /usr/lib/mxe/usr/i686-w64-mingw32.shared/qt5/bin
