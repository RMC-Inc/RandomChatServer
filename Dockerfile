FROM ubuntu:latest
RUN apt update
RUN apt install -y cmake
EXPOSE 8125:8125

COPY . /home
WORKDIR /home
RUN cmake cmake .
RUN make
RUN make install
ENTRYPOINT ["RandomChatServer"]