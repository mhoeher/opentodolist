#!/bin/bash

# Download secrets during CI and "install" them, e.g. import certificates
# into a new, temporary keychain and move provisioning profiles
# to the correct location.

set -e

# Fetch secure files - see https://docs.gitlab.com/ee/ci/secure_files/index.html#use-secure-files-in-cicd-jobs
if [ -n "$CI" ]; then
    export SECURE_FILES_DOWNLOAD_PATH=.secure-files
    curl --silent "https://gitlab.com/gitlab-org/incubation-engineering/mobile-devops/download-secure-files/-/raw/main/installer" | bash

    # We need to manually import certificates. On the build machine, export them
    # following the instructions here:
    # https://docs.github.com/en/actions/deployment/deploying-xcode-applications/installing-an-apple-certificate-on-macos-runners-for-xcode-development
    # Add them as secure files in the GitLab project settings.

    # The same needs to be done with provisioning profiles. Follow the instructions here:
    # https://ioscodesigning.io/exporting-code-signing-files/
    # The profiles can be found in ~/Library/MobileDevice/Provisioning Profiles/
    # after downloading them in Xcode.

    # Now, we can move everything in place following the instructions from the GitHub
    # documentation above:
    # create variables
    CERTIFICATE_PATH=$RUNNER_TEMP/build_certificate.p12
    PP_PATH=$RUNNER_TEMP/build_pp.mobileprovision

    KEYCHAIN_PATH=$PWD/$SECURE_FILES_DOWNLOAD_PATH/app-signing.keychain-db
    KEYCHAIN_PASSWORD="temporary-1234" # Gets discarded anyway - hence, this is as good as a generated one

    # create temporary keychain
    security create-keychain -p "$KEYCHAIN_PASSWORD" $KEYCHAIN_PATH
    security set-keychain-settings -lut 21600 $KEYCHAIN_PATH
    security unlock-keychain -p "$KEYCHAIN_PASSWORD" $KEYCHAIN_PATH

    # import certificate to keychain
    for cert in $PWD/$SECURE_FILES_DOWNLOAD_PATH/*.p12; do
        security import $cert -P "$APPLE_CERTIFICATES_PASSWORD" -A -t cert -f pkcs12 -k $KEYCHAIN_PATH
    done
    security list-keychain -d user -s $KEYCHAIN_PATH

    # apply provisioning profile
    mkdir -p ~/Library/MobileDevice/Provisioning\ Profiles
    cp $PWD/$SECURE_FILES_DOWNLOAD_PATH/*.provisionprofile ~/Library/MobileDevice/Provisioning\ Profiles
    cp $PWD/$SECURE_FILES_DOWNLOAD_PATH/*.mobileprovision ~/Library/MobileDevice/Provisioning\ Profiles
else
    XCODEBUILD_FLAGS=""
fi
