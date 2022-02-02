# This docker file is exactly the same as the official php:8.1-fpm container,
# but adds to it the trogdord extension.

# To build for 8.1: docker build -t crankycyclops/php8.1-fpm-trogdord-ext .
# To build for 8.0: docker build --build-arg PHP_VER=8.0 -t crankycyclops/php8.0-fpm-trogdord-ext .
# To build for 7.4: docker build --build-arg PHP_VER=7.4 -t crankycyclops/php7.4-fpm-trogdord-ext .

ARG PHP_VER=8.1
FROM php:${PHP_VER}-fpm

RUN apt-get update
RUN apt-get -y install g++ wget rapidjson-dev libasio-dev

WORKDIR /usr/local/src
RUN wget https://github.com/crankycyclops/trogdor-pp/archive/refs/tags/0.90.0.tar.gz
RUN tar -zvxpf 0.90.0.tar.gz
WORKDIR /usr/local/src/trogdor-pp-0.90.0/src/php7/trogdord

RUN phpize && ./configure && make -j$(nproc) && make install
RUN docker-php-ext-enable trogdord

# Cleanup after building
RUN apt-get remove -y g++ wget
RUN rm -rf /usr/local/src/trogdor-pp-0.90.0

WORKDIR /
ENTRYPOINT ["php-fpm"]
