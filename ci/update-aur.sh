#!/bin/bash

set -e

THIS_DIR="$(cd "$(dirname "$0")" && pwd)"

cd $THIS_DIR/../

# Install pre-requisites when running in CI
if [ -n "$CI" ]; then
    command -v ssh-agent >/dev/null || yum install -y --nogpgcheck openssh-clients
    command -v git >/dev/null || yum install -y --nogpgcheck git
    command -v which >/dev/null || yum install -y --nogpgcheck which
    command -v curl >/dev/null || yum install -y --nogpgcheck curl
fi

if [ -n "$CI" ]; then
    # Download secret files
    export SECURE_FILES_DOWNLOAD_PATH=.secure-files
    curl --silent "https://gitlab.com/gitlab-org/incubation-engineering/mobile-devops/download-secure-files/-/raw/main/installer" | bash
    chmod go-rwx $SECURE_FILES_DOWNLOAD_PATH/id_aur

    # Load SSH private key (required for release):
    eval $(ssh-agent -s)
    ssh-add $SECURE_FILES_DOWNLOAD_PATH/id_aur
fi

# Get latest tag (we do not include extra information, we release just on a plain
# tag granularity):
LATEST_TAG="$(git describe --tags | cut -f 1 -d '-')"

mkdir -p ~/.ssh
chmod 700 ~/.ssh

# Local testing only: remove the temporary git clone if present:
if [ -d opentodolist-aur ]; then
    rm -rf opentodolist-aur
fi

# Avoid git to promt us:
ssh-keyscan aur.archlinux.org >> ~/.ssh/known_hosts

# Clone the AUR repo:
if [ -n "$RELEASE_TO_AUR" ]; then
    # If we are building a tag, use the "writable" ssh URL:
    git clone ssh://aur@aur.archlinux.org/opentodolist.git opentodolist-aur
else
    # Otherwise, use the public https one:
    git clone https://aur.archlinux.org/opentodolist.git opentodolist-aur
fi

cd opentodolist-aur

# Set up git configuration:
git config user.email "martin@rpdev.net"
git config user.name "Martin Hoeher"

# Update release files:
cp ../build-arch/.SRCINFO .
cp ../PKGBUILD .

# Update version of PKGBUILD and include the pipeline which produced the file:
sed -i \
    -e "s/^pkgver=.*\$/pkgver=${LATEST_TAG}/g" \
    -e "/Maintainer: /a # Automatically released from ${CI_PIPELINE_URL}" \
    PKGBUILD

# Commit changes:
if git diff-index --quiet HEAD --; then
    echo "No changes"
    exit 0
fi

git commit -m "Update to v${LATEST_TAG}" .SRCINFO PKGBUILD

# Push updates
if [ -n "$RELEASE_TO_AUR" ]; then
    git push
fi
