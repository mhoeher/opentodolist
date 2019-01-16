FROM fedora
RUN \
    dnf install -y --nogpgcheck \
        flatpak \
        flatpak-builder \
        libappstream-glib \
    && \
    \
    flatpak remote-add \
    --if-not-exists \
        flathub https://flathub.org/repo/flathub.flatpakrepo \
    && \
    \
    flatpak install -y flathub org.kde.Platform//5.12 \
    && \
    \
    flatpak install -y flathub org.kde.Sdk//5.12

