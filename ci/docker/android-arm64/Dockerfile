FROM registry.gitlab.com/rpdev/opentodolist:android-base
ADD qt-installer-script.js /
RUN \
    wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run && \
    chmod +x qt-unified-linux-x64-online.run && \
    ./qt-unified-linux-x64-online.run --script qt-installer-script.js \
        --platform minimal --no-force-installations -v

ENV QT_ROOT /opt/Qt/5.12.3/android_arm64_v8a/
