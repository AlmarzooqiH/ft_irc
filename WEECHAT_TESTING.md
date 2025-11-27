# IRC Server Testing with WeeChat

## Build and Run WeeChat Docker Container

### Build the image:
```bash
docker build -t weechat-irc .
```

### Run WeeChat:
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

### 1. Add your server:
```
/server add myirc localhost/8080 -password=test123
```

### 2. Connect:
```
/connect myirc
```

### 3. Set your nickname (if not already set):
```
/nick yournickname
```

### 4. Join a channel:
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
/server add badpass localhost/8080 -password=wrongpass
/connect badpass
```

### Test multiple connections:
Open multiple terminals and run the container in each:
```bash
docker run -it --rm --network host weechat-irc
```
