FROM ubuntu:15.04

#ENV http_proxy 'http://157.92.49.223:8080/'
#ENV https_proxy 'http://157.92.49.223:8080/'
#ENV HTTP_PROXY 'http://157.92.49.223:8080/'
#ENV HTTPS_PROXY 'http://157.92.49.223:8080/'

RUN			apt-get update && apt-get install -y \
				wget \
				git \
				make \
				gcc \
				unzip \
				g++ \
				libpthread-stubs0-dev \
				libz-dev \
				cmake \
				python-pip \
				libssl-dev \
				valgrind

RUN			pip install requests
RUN			pip install gcovr

WORKDIR		/rocksdb
COPY		rocksdb.zip /rocksdb/
RUN			unzip rocksdb.zip && \
			cd rocksdb && \
			make static_lib

RUN			pip install gcovr

CMD /backend/makeAndRunServer.sh

EXPOSE 8000

