FROM ubuntu:15.04

RUN 	   apt-get update && apt-get install -y	wget \
						git \
						make \
						gcc \
						unzip
WORKDIR	   /backend
COPY	   5.6.zip /backend/
#RUN	   wget https://github.com/cesanta/mongoose/archive/5.6.zip \
RUN	   unzip 5.6.zip \
	&& cd mongoose-5.6/examples/restful_api \
	&& make \
	&& mv restful_api /usr/local/bin/restful_api

CMD ["/usr/local/bin/restful_api"]

EXPOSE 8000


	
