FROM ubuntu:latest
LABEL authors="saahi"

RUN apt-get update && apt-get install -y g++ make

WORKDIR /app

COPY . .

RUN g++ -o leader leader.cpp -lWs2_32

CMD [".\leader.exe"]