FROM gcc:latest

WORKDIR /app

COPY lab6.cpp /app/

RUN g++ -o lab6 lab6.cpp

CMD ["./lab6"]
