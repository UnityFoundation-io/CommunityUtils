# Copyright 2023 CommunityUtils Authors

ARG BASIS=ghcr.io/opendds/opendds:master
FROM $BASIS

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y && apt-get install -y wget automake autoconf libtool libmicrohttpd-dev

RUN wget https://github.com/curl/curl/releases/download/curl-7_87_0/curl-7.87.0.tar.gz && tar xzf curl-7.87.0.tar.gz && cd curl-7.87.0 && ./configure --with-openssl && make && make install

RUN git clone --depth 1 --branch 3.1 --single-branch https://github.com/JosephP91/curlcpp.git && cd curlcpp && mkdir build && cd build && cmake .. -DBUILD_SHARED_LIBS=SHARED && make && make install

RUN git clone --depth 1 --branch 0.18.2 --single-branch https://github.com/etr/libhttpserver.git && cd libhttpserver && ./bootstrap && mkdir build && cd build && ../configure && make && make install

ADD . /opt/CommunityUtils
ENV COMMUNITY_UTILS_ROOT=/opt/CommunityUtils
WORKDIR /opt/CommunityUtils

RUN . /opt/OpenDDS/setenv.sh && mwc.pl -type gnuace HSDS.mwc -include MPC/config && make && make install
