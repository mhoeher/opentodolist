FROM fedora
RUN \
    yum install -y \
        wget make cmake git java-1.8.0-openjdk java-1.8.0-openjdk-devel \
        libX11-xcb unzip extra-cmake-modules.noarch \
        ncurses-compat-libs gradle ninja-build \
        qt5-qtxmlpatterns-devel qt5-linguist appstream python \
        && \
    cd /opt && \
    mkdir android && \
    cd android && \
    mkdir android-sdk && \
    cd android-sdk && \
    wget https://dl.google.com/android/repository/sdk-tools-linux-4333796.zip && \
    unzip *.zip && \
    rm *.zip && \
    cd .. && \
    wget https://dl.google.com/android/repository/android-ndk-r19c-linux-x86_64.zip && \
    unzip *.zip && \
    rm *.zip && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | \
        ./android-sdk/tools/bin/sdkmanager --update && \
    (sleep 5 ; while true ; do sleep 1 ; printf 'y\r\n' ; done ) | \
        ./android-sdk/tools/bin/sdkmanager "platforms;android-26" "build-tools;27.0.3" && \
    yum clean all

ENV ANDROID_SDK_ROOT /opt/android/android-sdk
ENV ANDROID_NDK /opt/android/android-ndk-r19c
ENV ANDROID_NDK_ROOT /opt/android/android-ndk-r19c
ENV JAVA_HOME /usr/lib/jvm/java-1.8.0
