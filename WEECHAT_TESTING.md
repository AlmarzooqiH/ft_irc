# IRC Server Testing with WeeChat

## Setup Instructions

### Step 1: Start your IRC server on Mac terminal
```bash
make
./ircserv 8080 test123
```

Keep this terminal open.

### Step 2: Build WeeChat Docker image
In a new terminal:
```bash
docker build -t weechat-irc .
```

### Step 3: Run WeeChat in Docker

**For Mac/Windows:**
```bash
docker run -it --rm --add-host=irc.local:host-gateway weechat-irc
```

**For Linux:**
```bash
docker run -it --rm --network host weechat-irc
```

Or use the provided script:
```bash
chmod +x run_weechat.sh
./run_weechat.sh
```

## Connecting to Your IRC Server

Once WeeChat starts, use these commands:

### For Mac/Windows - use host.docker.internal:
```
/server add myirc host.docker.internal/8080 -password=test123
/connect myirc
```

### For Linux - use localhost:
```
/server add myirc localhost/8080 -password=test123
/connect myirc
```

### Set your nickname:
```
/nick yournickname
```

### Join a channel:
```
/join #test
```

## Useful WeeChat Commands

- `/quit` - Quit WeeChat
- `/disconnect` - Disconnect from server
- `/nick newnick` - Change nickname
- `/msg nickname message` - Send private message
- `/part` - Leave current channel
- `/help` - Show help

## Testing Different Scenarios

### Test wrong password:
```
/server add badpass host.docker.internal/8080 -password=wrongpass
/connect badpass
```

You should see: `464 * :Password incorrect`

### Test correct authentication:
```
/server add goodpass host.docker.internal/8080 -password=test123
/connect goodpass
/nick testuser
```

You should see welcome messages (001-004).

### Test multiple connections:
Open multiple terminals and run the container in each:
```bash
docker run -it --rm --add-host=irc.local:host-gateway weechat-irc
```

## Troubleshooting

### Can't connect from container to host?
- **Mac/Windows**: Make sure you use `host.docker.internal` not `localhost`
- **Linux**: Use `--network host` flag
- Check your IRC server is running with `ps aux | grep ircserv`
- Make sure port 8080 is not blocked by firewall

### Connection refused?
- Verify server is listening: `lsof -i :8080` or `netstat -an | grep 8080`
- Try connecting with nc first: `nc host.docker.internal 8080` (from inside container)

### Test from host machine first:
```bash
echo -e "PASS test123\nNICK testuser\nUSER test 0 * :Test\n" | nc localhost 8080
```
