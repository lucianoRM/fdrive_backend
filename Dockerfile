FROM ubuntu:15.04

RUN 	   apt-get update && apt-get install -y	wget \
						git \
						make \
						gcc \
						unzip \
						g++ \
						libpthread-stubs0-dev

WORKDIR	   /mongoose
COPY	   mongoose.5.6.zip /mongoose/
RUN	   unzip mongoose.5.6.zip \
	&& cd mongoose-5.6/examples/restful_api \
	&& make \
	&& mv restful_api /usr/local/bin/restful_api

WORKDIR	   /rocksdb
COPY	   rocksdb.zip /rocksdb/
RUN	   unzip rocksdb.zip \
	&& cd rocksdb \
	&& make shared_lib \
	&& mv librocksdb.so /usr/lib/ \
	&& mv librocksdb.so.3 /usr/lib/ \
	&& mv librocksdb.so.3.14 /usr/lib/ \
	&& mv librocksdb.so.3.14.0 /usr/lib/ \
	&& mv include/rocksdb /usr/include/rocksdb


CMD ["/usr/local/bin/restful_api"]

EXPOSE 8000

