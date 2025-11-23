# IRC Protocol Implementation - RFC 2812 Compliance

## Overview

This document explains how the server receives and parses IRC messages according to RFC 2812 specification.

## How IRC Messages Are Received

### 1. **Stream-Based Reception**
IRC messages arrive as a **continuous stream of bytes** over TCP sockets. Messages don't arrive as discrete units - they can be:
- Split across multiple `recv()` calls (partial messages)
- Multiple messages in a single `recv()` call
- Cut in the middle of a message

### 2. **Message Termination**
- Each IRC message is terminated with `\r\n` (CR-LF)
- Maximum message length: **512 characters** (including `\r\n`)
- Maximum usable content: **510 characters** (command + parameters)

### 3. **Message Format**
```
[":prefix "] command [params] \r\n
```

**Components:**
- **Prefix** (optional): Starts with `:`, indicates message origin (server/nickname)
- **Command**: IRC command (NICK, USER, JOIN, PRIVMSG, etc.) or 3-digit numeric
- **Parameters**: Up to **15 parameters** separated by spaces
  - Last parameter can contain spaces if prefixed with `:`
  
**Examples:**
```
NICK john\r\n
PASS secret123\r\n
USER john 0 * :John Doe\r\n
:server.com 001 john :Welcome to IRC\r\n
PRIVMSG #channel :Hello everyone!\r\n
```

## Implementation Changes

### New Files Created

#### 1. `includes/Message.hpp` & `src/Message.cpp`
A complete IRC message parser that:
- Extracts the optional prefix
- Identifies the command
- Parses up to 15 parameters
- Handles trailing parameters with spaces (`:` prefix)
- Validates message format

**Key Methods:**
```cpp
bool parse(const std::string& rawMsg);        // Parse raw message
std::string getCommand() const;                // Get command
std::vector<std::string> getParameters() const; // Get all params
std::string getParameter(size_t index) const;  // Get specific param
bool isValid() const;                          // Check if parsing succeeded
```

### Modified Files

#### 2. `src/Server.cpp`
**New Methods Added:**

```cpp
void handleMessage(pollfd& client, const std::string& rawMessage);
```
- Replaces simple string matching with proper RFC 2812 parsing
- Creates a `Message` object to parse the raw message
- Validates message format
- Delegates to `processCommand()`

```cpp
void processCommand(int clientFd, const std::string& command, 
                    const std::vector<std::string>& params);
```
- Handles individual IRC commands (CAP, PASS, NICK, USER, etc.)
- Sends appropriate error responses for invalid input
- Case-insensitive command matching
- Validates parameter counts

**Modified Method:**
- `start()`: Updated message processing loop to use `handleMessage()` instead of `performHandshake()`

#### 3. `includes/Server.hpp`
Added method declarations:
```cpp
void handleMessage(pollfd& client, const std::string& rawMessage);
void processCommand(int clientFd, const std::string& command, 
                    const std::vector<std::string>& params);
```

## How It Works Now

### Message Reception Flow

```
1. TCP Socket receives bytes
   ↓
2. recv() stores in buffer (may be partial)
   ↓
3. Append to clientBuffer[fd] (persistent per client)
   ↓
4. Search for \r\n delimiter
   ↓
5. If found: Extract complete message
   ↓
6. Parse using Message class
   ↓
7. Process command with parameters
   ↓
8. Remove processed message from buffer
   ↓
9. Repeat until no more \r\n found
   ↓
10. Remaining data stays in buffer for next recv()
```

### Example: Parsing `USER john 0 * :John Doe\r\n`

```cpp
Message msg("USER john 0 * :John Doe");

msg.getCommand();           // Returns: "USER"
msg.getParameters();        // Returns: ["john", "0", "*", "John Doe"]
msg.getParameter(0);        // Returns: "john"
msg.getParameter(3);        // Returns: "John Doe" (with space!)
msg.getParameterCount();    // Returns: 4
```

### Example: Parsing `:server.com 001 nick :Welcome\r\n`

```cpp
Message msg(":server.com 001 nick :Welcome");

msg.getPrefix();            // Returns: "server.com"
msg.getCommand();           // Returns: "001"
msg.getParameter(0);        // Returns: "nick"
msg.getParameter(1);        // Returns: "Welcome"
```

## Supported Commands

Currently implemented in `processCommand()`:

| Command | Parameters | Description |
|---------|-----------|-------------|
| CAP     | LS/REQ/END | Capability negotiation |
| PASS    | <password> | Server password |
| NICK    | <nickname> | Set nickname |
| USER    | <user> <mode> <unused> <realname> | User registration |

### Error Responses

The server sends RFC-compliant numeric replies:

- `421`: Unknown command
- `431`: No nickname given
- `461`: Not enough parameters
- `464`: Password incorrect

Format: `:servername numeric client :message\r\n`

## Benefits of This Implementation

### ✅ RFC 2812 Compliant
- Proper message parsing according to specification
- Supports all valid IRC message formats
- Handles edge cases (empty params, trailing params, etc.)

### ✅ Robust Buffering
- Handles partial messages correctly
- Processes multiple messages in one buffer
- No data loss between recv() calls

### ✅ Extensible
- Easy to add new commands
- Clean separation: parsing vs. processing
- Reusable Message class for any IRC message

### ✅ Parameter Handling
- Correctly splits parameters
- Preserves spaces in trailing parameters
- Validates parameter counts per command

## Testing Your Implementation

### Test with netcat:
```bash
nc localhost <port>
PASS yourpassword
NICK testuser
USER testuser 0 * :Test User
```

### Test with trailing parameters:
```bash
PRIVMSG #channel :This message has spaces
```

### Test partial messages:
The server should correctly handle messages split across multiple TCP packets.

## Next Steps

To complete your IRC server, you should:

1. **Add more command handlers** in `processCommand()`:
   - JOIN (join channels)
   - PART (leave channels)
   - PRIVMSG (send messages)
   - QUIT (disconnect)
   - PING/PONG (keep-alive)
   
2. **Implement channel management**:
   - Channel list storage
   - User-to-channel mapping
   - Channel modes and operators

3. **Add authentication state tracking**:
   - Track which commands clients have sent
   - Only allow certain commands after authentication
   - Send welcome messages (001-004 numerics)

4. **Validate input**:
   - Nickname format validation
   - Channel name validation  
   - Message length limits

5. **Implement message broadcasting**:
   - Send PRIVMSG to channel members
   - Notify users of JOIN/PART events

## References

- **RFC 2812**: Internet Relay Chat: Client Protocol
  - Section 2.3: Message format
  - Section 2.3.1: Augmented BNF
  
- **Message Format**:
  ```
  message = [ ":" prefix SPACE ] command [ params ] crlf
  params  = *14( SPACE middle ) [ SPACE ":" trailing ]
  ```

## Key Takeaways

🔑 **Messages arrive as streams, not packets** - Always buffer incomplete data

🔑 **Messages are terminated by `\r\n`** - This is how you know where one ends

🔑 **Maximum 512 chars** - Including the `\r\n` terminator

🔑 **Up to 15 parameters** - Last one can have spaces if prefixed with `:`

🔑 **Prefix is optional** - Servers add it; clients usually don't send it

🔑 **Commands are case-insensitive** - NICK = Nick = nick
