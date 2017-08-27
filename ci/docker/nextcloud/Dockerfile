FROM nextcloud:apache

RUN \
    /entrypoint.sh php /var/www/html/occ maintenance:install \
    --database sqlite \
    --admin-user admin \
    --admin-pass admin && \
    chown -R www-data:www-data /var/www/html && \
    rsync -a /var/www/html/ /usr/nextcloud_ci/
ADD entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
CMD ["apache2-foreground"]
