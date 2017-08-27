#!/bin/bash
rsync -a /usr/nextcloud_ci/ /var/www/html/
exec "$@"

