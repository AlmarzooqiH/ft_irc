# Quick Reference Card - IRC Message Protocol

## 📋 Message Format (RFC 2812)

```
[":prefix "] command [params] \r\n
```

### Rules:
- ✅ Max **512 chars** total (including `\r\n`)
- ✅ Max **510 chars** of content
- ✅ Max **15 parameters**
- ✅ Space (0x20) separates parts
- ✅ Last param can have spaces if prefixed with `:`
- ✅ Prefix is optional, starts with `:`
- ✅ Commands are case-insensitive

---

## 🔍 Parsing Examples

| Raw Message | Parsed Result |
|-------------|---------------|
| `PING\r\n` | cmd=`PING`, params=`[]` |
| `NICK john\r\n` | cmd=`NICK`, params=`["john"]` |
| `USER john 0 * :John Doe\r\n` | cmd=`USER`, params=`["john", "0", "*", "John Doe"]` |
| `:server.com 001 nick :Welcome\r\n` | prefix=`server.com`, cmd=`001`, params=`["nick", "Welcome"]` |
| `PRIVMSG #ch :Hi!\r\n` | cmd=`PRIVMSG`, params=`["#ch", "Hi!"]` |

---

## 💻 Code Usage

### Parse a Message:
```cpp
#include "includes/Message.hpp"

Message msg("USER john 0 * :John Doe");

if (msg.isValid()) {
    std::string cmd = msg.getCommand();        // "USER"
    std::string user = msg.getParameter(0);    // "john"
    std::string name = msg.getParameter(3);    // "John Doe"
    size_t count = msg.getParameterCount();    // 4
}
```

### In Your Server:
```cpp
// Already implemented in Server::handleMessage()
void handleMessage(pollfd& client, const std::string& rawMessage) {
    Message msg(rawMessage);
    if (msg.isValid()) {
        processCommand(client.fd, msg.getCommand(), msg.getParameters());
    }
}
```

---

## 🎯 Common IRC Commands

| Command | Format | Purpose |
|---------|--------|---------|
| **PASS** | `PASS <password>` | Authenticate |
| **NICK** | `NICK <nickname>` | Set nickname |
| **USER** | `USER <user> <mode> <unused> <realname>` | Register user |
| **JOIN** | `JOIN <channel> [key]` | Join channel |
| **PART** | `PART <channel> [:reason]` | Leave channel |
| **PRIVMSG** | `PRIVMSG <target> :<message>` | Send message |
| **PING** | `PING <server>` | Keep-alive |
| **PONG** | `PONG <server>` | Response to PING |
| **QUIT** | `QUIT [:reason]` | Disconnect |

---

## 🔢 Common Numeric Replies

| Code | Name | Format |
|------|------|--------|
| **001** | RPL_WELCOME | `:server 001 nick :Welcome...` |
| **421** | ERR_UNKNOWNCOMMAND | `:server 421 nick CMD :Unknown command` |
| **431** | ERR_NONICKNAMEGIVEN | `:server 431 nick :No nickname given` |
| **461** | ERR_NEEDMOREPARAMS | `:server 461 nick CMD :Not enough parameters` |
| **464** | ERR_PASSWDMISMATCH | `:server 464 nick :Password incorrect` |

---

## 📊 Server Data Flow

```
Client sends data
      ↓
recv() stores in buffer
      ↓
clientBuffer[fd] accumulates
      ↓
Search for \r\n
      ↓
Extract complete message
      ↓
handleMessage()
      ↓
Message.parse()
      ↓
processCommand()
      ↓
Execute command logic
      ↓
Send response to client
```

---

## 🧪 Testing

### Manual Test (with netcat):
```bash
$ nc localhost 6667
PASS secret
NICK john
USER john 0 * :John Doe
JOIN #test
PRIVMSG #test :Hello!
QUIT :Bye
```

### Automated Test:
```bash
$ ./test_parser
# Should show ✅ for all valid messages
```

---

## 🚨 Common Pitfalls

❌ **Don't forget `\r\n`** - Messages must end with both CR and LF

❌ **Don't hardcode positions** - Use the Message parser, not `substr()`

❌ **Don't ignore buffering** - Messages may arrive in chunks

❌ **Don't exceed 512 chars** - IRC protocol limit (including `\r\n`)

❌ **Don't forget trailing param** - Use `:` for params with spaces

✅ **Do use Message class** - It handles all edge cases correctly

✅ **Do buffer incomplete messages** - Your server already does this!

✅ **Do validate parameters** - Check count before accessing

✅ **Do handle case-insensitively** - NICK = Nick = nick

---

## 📁 File Overview

| File | Purpose |
|------|---------|
| `includes/Message.hpp` | Message parser declaration |
| `src/Message.cpp` | RFC 2812 parser implementation |
| `src/Server.cpp` | Server with `handleMessage()` |
| `test_message_parser.cpp` | Parser tests (all pass ✅) |
| `IRC_MESSAGE_PARSING.md` | Full documentation |
| `BEFORE_AFTER_COMPARISON.md` | Old vs new comparison |
| `IMPLEMENTATION_SUMMARY.md` | Complete summary |

---

## 🔧 Adding New Commands

```cpp
// In Server::processCommand():

if (cmd == "YOURCOMMAND") {
    // 1. Validate parameter count
    if (params.size() < MIN_PARAMS) {
        std::string error = ":HAI 461 * YOURCOMMAND :Not enough parameters\r\n";
        send(clientFd, error.c_str(), error.length(), MSG_DONTWAIT);
        return;
    }
    
    // 2. Extract parameters
    std::string param1 = params[0];
    std::string param2 = params[1];
    
    // 3. Execute command logic
    // ... your code here ...
    
    // 4. Send response
    std::string response = ":HAI RESPONSE :Success\r\n";
    send(clientFd, response.c_str(), response.length(), MSG_DONTWAIT);
}
```

---

## 🎓 Key Takeaways

1. IRC messages are **stream-based**, not packet-based
2. Always **buffer incomplete messages** until you find `\r\n`
3. **Parse properly** - don't use hardcoded string positions
4. **Validate everything** - parameter counts, formats, etc.
5. Messages can have **up to 15 parameters**
6. Last parameter can **contain spaces** if prefixed with `:`
7. Commands are **case-insensitive**
8. Maximum message length is **512 characters**

---

**Print this card and keep it handy! 📌**
