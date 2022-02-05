# This docker file is exactly the same as the official php:8.1-fpm container,
# but adds to it the trogdord extension.

# To build for 8.1: docker build -t crankycyclops/php8.1-fpm-trogdord-ext:0.90.0 .
# To build for 8.0: docker build --build-arg PHP_VER=8.0 -t crankycyclops/php8.0-fpm-trogdord-ext:0.90.0 .
# To build for 7.4: docker build --build-arg PHP_VER=7.4 -t crankycyclops/php7.4-fpm-trogdord-ext:0.90.0 .

# To build a version that includes npm (required to bootstrap newly installed Laraval applications):

# docker build --build-arg PHP_VER=8.1 --build-arg INCLUDE_NPM=true -t crankycyclops/php8.1-fpm-trogdord-ext-npm:0.90.0 .
# docker build --build-arg PHP_VER=8.0 --build-arg INCLUDE_NPM=true -t crankycyclops/php8.0-fpm-trogdord-ext-npm:0.90.0 .
# docker build --build-arg PHP_VER=7.4 --build-arg INCLUDE_NPM=true -t crankycyclops/php7.4-fpm-trogdord-ext-npm:0.90.0 .

# Determines which version of PHP we're building the container for
ARG PHP_VER=8.1

# Determines whether or not we add support for npm (disabled by default)
ARG INCLUDE_NPM=false

FROM php:${PHP_VER}-fpm

# See: https://stackoverflow.com/a/56748289
ARG INCLUDE_NPM

RUN apt-get update
RUN apt-get -y install g++ wget rapidjson-dev libasio-dev

WORKDIR /usr/local/src
RUN wget https://github.com/crankycyclops/trogdor-pp/archive/refs/tags/0.90.0.tar.gz
RUN tar -zvxpf 0.90.0.tar.gz
WORKDIR /usr/local/src/trogdor-pp-0.90.0/src/php7/trogdord

RUN phpize && ./configure && make -j$(nproc) && make install
RUN docker-php-ext-enable trogdord

# Install composer
WORKDIR /usr/bin
RUN wget -c https://getcomposer.org/download/latest-stable/composer.phar
RUN mv composer.phar composer
RUN chmod 755 composer

# Optionally install npm
ENV NVM_DIR /usr/local/nvm
ENV NODE_VERSION 16.13.2
RUN if [ "true" = "$INCLUDE_NPM" ]; then \
    mkdir /usr/local/nvm; \
    wget -qO- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash; \
    . $NVM_DIR/nvm.sh; \
    nvm install $NODE_VERSION; \
    nvm alias default $NODE_VERSION; \
    nvm use default; \
    echo "PATH=\$PATH:$NVM_DIR/v$NODE_VERSION/bin" >> /etc/profile; \
fi

# Cleanup after building
RUN apt-get remove -y g++ wget
RUN rm -rf /usr/local/src/trogdor-pp-0.90.0

WORKDIR /
ENTRYPOINT ["php-fpm"]
