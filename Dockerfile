FROM alpine
WORKDIR /app
COPY . .
RUN apk add build-base
RUN mkdir build
RUN make
RUN chmod +x /app/server
EXPOSE 5002
ENTRYPOINT ["/app/server"]
