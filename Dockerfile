FROM ubuntu:15.04

RUN			apt-get update && apt-get install -y
				wget \
				git \
				make \
				gcc \
				unzip \
				g++ \
				libpthread-stubs0-dev

COPY		src /src
WORKDIR		/src
RUN			make


#WORKDIR	   /rocksdb
#COPY	   rocksdb.zip /rocksdb/
#RUN	   unzip rocksdb.zip \
#	&& cd rocksdb \
#	&& make shared_lib \
#	&& mv librocksdb.so /usr/lib/ \
#	&& mv librocksdb.so.3 /usr/lib/ \
#	&& mv librocksdb.so.3.14 /usr/lib/ \
#	&& mv librocksdb.so.3.14.0 /usr/lib/ \
#	&& mv include/rocksdb /usr/include/rocksdb


CMD ["/src/restful_api"]

EXPOSE 8000

