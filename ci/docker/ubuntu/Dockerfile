FROM ubuntu:trusty
RUN \
    apt-get update && \
    apt-get install -y software-properties-common wget curl && \
    add-apt-repository --yes ppa:beineri/opt-qt591-trusty && \
    wget http://ftp.de.debian.org/debian/pool/main/p/patchelf/patchelf_0.8-2_amd64.deb && \
    dpkg -i patchelf_0.8-2_amd64.deb && \
    rm patchelf_0.8-2_amd64.deb && \
    apt-get update && \
    apt-get install -y \
        qt59base \
        qt59declarative \
        qt59xmlpatterns \
        qt59script \
        qt59tools \
        qt59multimedia \
        qt59svg \
        qt59graphicaleffects \
        qt59imageformats \
        qt59translations \
        qt59quickcontrols \
        qt59quickcontrols2 \
        qt59sensors \
        qt59serialbus \
        qt59serialport \
        qt59x11extras \
        qt59connectivity \
        qt59location \
        qt59websockets \
        qt59webchannel \
        qt593d \
        qt59canvas3d \
        qt59scxml \
        qt59gamepad \
        qt59webengine \
        qt59speech \
        qt59remoteobjects \
        build-essential \
        libgl1-mesa-dev \
        libfuse2 \
        desktop-file-utils && \
    apt-get clean
