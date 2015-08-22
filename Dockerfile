FROM ubuntu:15.04

RUN 	   apt-get update && apt-get install -y	wget \
						git \
						make \
						gcc \
						unzip
RUN	   mkdir /backend
WORKDIR	   /backend
RUN	   wget https://github.com/cesanta/mongoose/archive/5.6.zip \
	&& unzip 5.6 \
	&& cd mongoose-5.6/examples/web_server \
	&& make \
	&& mv web_server /backend/mongoose

CMD ["/backend/mongoose"]

EXPOSE 8080


	
