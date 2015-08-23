FROM ubuntu:15.04

RUN 	   apt-get update && apt-get install -y	wget \
						git \
						make \
						gcc \
						unzip
WORKDIR	   /usr/local
RUN	   wget https://github.com/cesanta/mongoose/archive/5.6.zip \
	&& unzip 5.6 \
	&& cd mongoose-5.6/examples/web_server \
	&& make \
	&& mv web_server /usr/local/bin/mongoose

CMD ["/usr/local/bin/mongoose"]

EXPOSE 8080


	
