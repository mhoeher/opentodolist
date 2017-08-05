FROM fedora

ENV BASE_URL https://download.nextcloud.com/server/releases/
ENV NEXTCLOUD_RELEASE nextcloud-12.0.0

RUN dnf makecache && \
    dnf install -y --nogpgcheck \
        php \
        php-pecl-zip \
        php-xml \
        php-mbstring \
        php-gd \
        php-posix \
        php-pdo \
        httpd \
        bzip2 && \
    dnf clean all && \
    curl -o nextcloud-12.0.0.tar.bz2 \
        ${BASE_URL}${NEXTCLOUD_RELEASE}.tar.bz2 && \
    curl -o ${NEXTCLOUD_RELEASE}.tar.bz2.sha256 \
        ${BASE_URL}${NEXTCLOUD_RELEASE}.tar.bz2.sha256 && \
    sha256sum -c ${NEXTCLOUD_RELEASE}.tar.bz2.sha256 && \
    bunzip2 ${NEXTCLOUD_RELEASE}.tar.bz2 && \
    rm ${NEXTCLOUD_RELEASE}.tar.bz2.sha256 && \
    tar xf ${NEXTCLOUD_RELEASE}.tar && \
    rm ${NEXTCLOUD_RELEASE}.tar && \
    rm -r /var/www/html && \
    mv  nextcloud /var/www/html && \
    php /var/www/html/occ maintenance:install && \
        --database sqlite \
        --admin-user admin \
        --admin-pass admin && \
    chown -R apache:apache /var/www/html

ENTRYPOINT httpd -D FOREGROUND
