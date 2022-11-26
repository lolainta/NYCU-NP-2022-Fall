# lab4

## Build Containers

You can use docker-compose to build the containers for test cases.

```shell
docker-compose build
docker-compose up
```

## Testing

You can use netcat `nc` to test those command specified in `docker-compose.yml`

```shell
nc localhost 10001
nc localhost 10002
nc localhost 10003
nc localhost 10004
nc localhost 10005
```
