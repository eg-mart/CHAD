FROM alpine
WORKDIR /app
COPY . .
RUN apk add build-base
RUN gcc -o server server.c
RUN chmod +x /app/server
EXPOSE 5002
ENTRYPOINT ["/app/server"]
