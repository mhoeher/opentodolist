FROM fedora
ADD qt-installer-script.js /
RUN \
    yum install -y \
        wget make cmake git java-1.8.0-openjdk java-1.8.0-openjdk-devel \
        libX11-xcb unzip extra-cmake-modules.noarch \
        ncurses-compat-libs gradle \
        && \
    wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run && \
    chmod +x qt-unified-linux-x64-online.run && \
    ./qt-unified-linux-x64-online.run --script qt-installer-script.js \
        --platform minimal --no-force-installations -v && \
    rm -f qt-unified-linux-x64-online.run && \
    cd /opt &&
    mkdir android && \
    cd android && \
    wget https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip && \
    wget https://dl.google.com/android/repository/android-ndk-r16b-linux-x86_64.zip && \
    unzip sdk-tools-linux-3859397.zip && \
    unzip android-ndk-r16b-linux-x86_64.zip && \
    rm *.zip && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | ./tools/bin/sdkmanager --update && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | ./tools/bin/sdkmanager "platforms;android-26" "build-tools;27.0.3" && \
    yum clean all
