#!/usr/bin/env bash

if pgrep -f "Docker" > /dev/null; then
	echo "Stopping Docker gracefully..."
	osascript -e 'quit app "Docker"' 2>/dev/null
	
	# Wait for Docker to stop
	local timeout=30
	while pgrep -f "Docker" > /dev/null && [ $timeout -gt 0 ]; do
		sleep 1
		timeout=$((timeout - 1))
	done
	
	# Force kill if still running
	if pgrep -f "Docker" > /dev/null; then
		echo "Force killing Docker processes..."
		pkill -f "Docker"
	fi
fi

# Clean up any remaining Docker processes
pkill -f "com.docker" 2>/dev/null || true

rm -rf /Users/$USER/goinfre/com.docker.docker /Users/$USER/goinfre/docker # remove the docker folders in the goinfre directory
mkdir -p /Users/$USER/goinfre/com.docker.docker # create the docker folder in the goinfre directory
# create a soft link between the created folder and a folder in the Containers directory
ln -s /Users/$USER/goinfre/com.docker.docker /Users/$USER/Library/Containers/com.docker.docker
open -a Docker