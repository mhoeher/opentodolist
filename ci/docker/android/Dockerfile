FROM fedora
ADD qt-installer-script.js /
RUN \
    yum install -y \
        wget make cmake git java-1.8.0-openjdk java-1.8.0-openjdk-devel \
        libX11-xcb unzip extra-cmake-modules.noarch \
        ncurses-compat-libs gradle ninja-build \
        && \
    wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run && \
    chmod +x qt-unified-linux-x64-online.run && \
    ./qt-unified-linux-x64-online.run --script qt-installer-script.js \
        --platform minimal --no-force-installations -v && \
    rm -f qt-unified-linux-x64-online.run && \
    cd /opt && \
    mkdir android && \
    cd android && \
    mkdir android-sdk && \
    cd android-sdk && \
    wget https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip && \
    unzip *.zip && \
    rm *.zip && \
    cd .. && \
    wget https://dl.google.com/android/repository/android-ndk-r15c-linux-x86_64.zip && \
    unzip *.zip && \
    rm *.zip && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | \
        ./android-sdk/tools/bin/sdkmanager --update && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | \
        ./android-sdk/tools/bin/sdkmanager "platforms;android-26" "build-tools;27.0.3" && \
    yum clean all

ENV ANDROID_SDK_ROOT /opt/android/android-sdk
ENV ANDROID_NDK /opt/android/android-ndk-r15c
ENV ANDROID_NDK_ROOT /opt/android/android-ndk-r15c
ENV QT_ARM_ROOT /opt/Qt/5.11.2/android_armv7/
ENV QT_X86_ROOT /opt/Qt/5.11.2/android_x86/
ENV JAVA_HOME /usr/lib/jvm/java-1.8.0
