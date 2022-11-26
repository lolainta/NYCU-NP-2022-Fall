FROM gcc

WORKDIR /lab5

COPY ./src .

RUN g++ -Wall lab5.cpp -o server

EXPOSE 10005
ENTRYPOINT ./server 10005
