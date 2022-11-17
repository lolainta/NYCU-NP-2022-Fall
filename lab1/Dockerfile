FROM alpine

RUN apk add fish python3 scapy libpcap

WORKDIR /lab1

COPY ./src .

ENTRYPOINT ./ans.fish
