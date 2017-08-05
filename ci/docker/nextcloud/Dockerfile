FROM fedora

ENV BASE_URL https://download.nextcloud.com/server/releases/
ENV NEXTCLOUD_RELEASE nextcloud-12.0.0

RUN dnf makecache
RUN dnf install -y --nogpgcheck \
        php \
        php-pecl-zip \
        php-xml \
        php-mbstring \
        php-gd \
        php-posix \
        php-pdo \
        httpd \
        bzip2
RUN dnf clean all
RUN curl -o nextcloud-12.0.0.tar.bz2 \
        ${BASE_URL}${NEXTCLOUD_RELEASE}.tar.bz2
RUN curl -o ${NEXTCLOUD_RELEASE}.tar.bz2.sha256 \
        ${BASE_URL}${NEXTCLOUD_RELEASE}.tar.bz2.sha256
RUN sha256sum -c ${NEXTCLOUD_RELEASE}.tar.bz2.sha256
RUN bunzip2 ${NEXTCLOUD_RELEASE}.tar.bz2
RUN rm ${NEXTCLOUD_RELEASE}.tar.bz2.sha256
RUN tar xf ${NEXTCLOUD_RELEASE}.tar
RUN rm ${NEXTCLOUD_RELEASE}.tar
RUN rm -r /var/www/html
RUN mv  nextcloud /var/www/html
RUN php /var/www/html/occ maintenance:install \
        --database sqlite \
        --admin-user admin \
        --admin-pass admin
RUN chown -R apache:apache /var/www/html

ENTRYPOINT httpd -D FOREGROUND
