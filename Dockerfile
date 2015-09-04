FROM ubuntu:15.04

ENV http_proxy 'http://157.92.49.223:8080/'
ENV https_proxy 'http://157.92.49.223:8080/'
ENV HTTP_PROXY 'http://157.92.49.223:8080/'
ENV HTTPS_PROXY 'http://157.92.49.223:8080/'

RUN			apt-get update && apt-get install -y \
				wget \
				git \
				make \
				gcc \
				unzip \
				g++ \
				libpthread-stubs0-dev \
				libz-dev

RUN			mkdir /src

COPY		src /src

WORKDIR		/rocksdb
COPY		rocksdb.zip /rocksdb/
RUN			unzip rocksdb.zip && \
			cd rocksdb && \
			make static_lib && \
			mv librocksdb.a /src/

WORKDIR		/src
RUN			make


CMD ["/src/fdrive"]

EXPOSE 8000

