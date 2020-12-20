FROM ubuntu:20.04

ENV DEBIAN_FRONTEND="noninteractive" TZ="America/Los_Angeles"
RUN apt-get update && apt-get -y install bash wget git g++ cmake libasio-dev rapidjson-dev libhiredis-dev libxml2 libxml2-dev liblua5.3 liblua5.3-dev libinih1 libinih-dev redis-server
RUN cd /usr/include && wget https://raw.githubusercontent.com/onqtam/doctest/2.4.0/doctest/doctest.h

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

# test_trogdord needs to run as a non-root user because it relies on file
# permissions that don't apply to root.
RUN groupadd -r app &&\
    useradd -r -g app -d /home/app -s /sbin/nologin -c "Docker image user" app
RUN chown -R app:app /home/app
USER app

ENTRYPOINT ["/entrypoint.sh"]