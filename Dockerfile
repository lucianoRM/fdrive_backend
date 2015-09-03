FROM ubuntu:15.04

RUN			apt-get update && apt-get install -y \
				wget \
				git \
				make \
				gcc \
				unzip \
				g++ \
				libpthread-stubs0-dev

WORKDIR		/rocksdb
COPY		rocksdb.zip /rocksdb/
RUN			unzip rocksdb.zip && \
			cd rocksdb && \
			make static_lib && \
			mv librocksdb.a /src/

COPY		src /src
WORKDIR		/src
RUN			make


CMD ["/src/restful_api"]

EXPOSE 8000

