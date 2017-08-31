FROM ubuntu:trusty
RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    add-apt-repository --yes ppa:beineri/opt-qt591-trusty && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    sudo dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        qt59-meta-full build-essential \
        libgl1-mesa-dev libfuse2 desktop-file-utils && \
    apt-get clean
