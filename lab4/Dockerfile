FROM gcc

WORKDIR /lab4

COPY ./src .

RUN g++ -Wall lab4.cpp -o nkat

ENTRYPOINT ./nkat 11111 ${TEST_COMMAND}
