FROM gcc

WORKDIR /lab2

COPY ./src .
COPY script.sh .
RUN chmod +x script.sh

RUN g++ -Wall lab2.cpp -o unpacker

ENTRYPOINT ./script.sh
