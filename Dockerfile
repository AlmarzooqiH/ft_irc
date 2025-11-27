FROM alpine:latest

RUN apk add --no-cache \
    weechat \
    weechat-perl \
    weechat-python \
    ca-certificates

WORKDIR /root

ENTRYPOINT ["weechat"]
