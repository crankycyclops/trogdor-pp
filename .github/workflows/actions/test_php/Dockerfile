FROM ubuntu:20.04

# Add PHP repository
RUN apt-get update && apt-get install -y software-properties-common
RUN LC_ALL=C.UTF-8 add-apt-repository -y ppa:ondrej/php && apt-get update

# Prevents the script from stalling due to requiring the user to pick a timezone
RUN export DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true
RUN apt-get install -y tzdata

# Install build dependencies for each supported PHP version (if I don't install PHP 8.0 first, I get an error about SIZEOF_SIZE_T not being defined) when building my extension.)
RUN apt-get update
RUN apt-get -y install php8.0-cli php8.0-dev
RUN apt-get -y install git g++ libasio-dev rapidjson-dev php7.2-cli php7.2-dev php7.3-cli php7.3-dev php7.4-cli php7.4-dev

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
