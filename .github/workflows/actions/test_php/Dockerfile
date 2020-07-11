FROM ubuntu:18.04

RUN apt-get update && apt-get install -y software-properties-common
RUN LC_ALL=C.UTF-8 add-apt-repository -y ppa:ondrej/php && apt-get update
RUN apt-get update && apt-get -y install git g++ php7.2-cli php7.2-dev php7.3-cli php7.3-dev php7.4-cli php7.4-dev

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]