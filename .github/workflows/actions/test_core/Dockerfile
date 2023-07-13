FROM ubuntu:22.04

ENV DEBIAN_FRONTEND="noninteractive" TZ="America/Los_Angeles"
RUN apt-get update && apt-get -y install wget git g++ cmake libxml2 libxml2-dev liblua5.1 liblua5.1-dev liblua5.2 liblua5.2-dev liblua5.3 liblua5.3-dev
RUN cd /usr/include && wget https://raw.githubusercontent.com/onqtam/doctest/v2.4.11/doctest/doctest.h

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]