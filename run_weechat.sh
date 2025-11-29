#!/bin/bash

# Build the Docker image
docker build -t weechat-irc .

# Run WeeChat in a container
# The container will connect to the host's IRC server
docker run -it --rm \
    --network host \
    weechat-irc
