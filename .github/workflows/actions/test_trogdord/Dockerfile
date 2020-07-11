FROM ubuntu:18.04

RUN apt-get update && apt-get -y install wget git g++ cmake libboost-all-dev libhiredis-dev libxml2 libxml2-dev liblua5.3 liblua5.3-dev
RUN cd /usr/include && wget https://raw.githubusercontent.com/onqtam/doctest/2.4.0/doctest/doctest.h

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]