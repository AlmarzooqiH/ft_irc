#!/bin/bash

# Build the Docker image
docker build -t weechat-irc .

# Run WeeChat in a container
# On Mac, use host.docker.internal to reach the host machine
docker run -it --rm \
    --add-host=irc.local:host-gateway \
    weechat-irc
