# HAI IRC Server - Complete Features Documentation

## Overview
HAI IRC Server is a fully RFC 2812 compliant Internet Relay Chat server implementation in C++98. This document provides a comprehensive overview of all implemented features with examples and usage guidelines.

---

## Table of Contents
1. [Connection and Registration](#connection-and-registration)
2. [Channel Operations](#channel-operations)
3. [Channel Modes](#channel-modes)
4. [User Communication](#user-communication)
5. [Channel Management](#channel-management)
6. [Server Information](#server-information)
7. [Testing Examples](#testing-examples)

---

## Connection and Registration

### PASS - Server Password Authentication
**RFC 2812 Section 3.1.1**

```irc
PASS <password>
```

Must be sent before NICK/USER registration.

**Example:**
```irc
PASS secretpassword
```

### NICK - Set Nickname
**RFC 2812 Section 3.1.2**

```irc
NICK <nickname>
```

Set or change your nickname (max 9 characters).

**Example:**
```irc
NICK alice
```

**Replies:**
- `433 ERR_NICKNAMEINUSE` - Nickname already in use
- `432 ERR_ERRONEUSNICKNAME` - Invalid nickname format

### USER - User Registration
**RFC 2812 Section 3.1.3**

```irc
USER <username> <mode> <unused> :<realname>
```

Complete user registration with username and real name.

**Example:**
```irc
USER alice 0 * :Alice Wonderland
```

### Registration Flow
```irc
PASS mypassword
NICK alice
USER alice 0 * :Alice Wonderland
# Server sends: 001-004 welcome messages
```

---

## Channel Operations

### JOIN - Join Channels
**RFC 2812 Section 3.2.1**

```irc
JOIN <channel>[,<channel>] [<key>[,<key>]]
JOIN 0  (leave all channels)
```

**Features:**
- Dynamic channel creation
- Multi-channel joins with keys
- Special `JOIN 0` to leave all channels

**Examples:**
```irc
JOIN #general                    # Join channel
JOIN #secured mykey              # Join with password
JOIN #foo,#bar key1,key2         # Join multiple channels
JOIN 0                           # Leave all channels
```

**Automatic Behaviors:**
- First user becomes channel operator (@)
- Sends topic and names list after successful join

**Error Replies:**
- `471 ERR_CHANNELISFULL` - Channel at user limit (+l)
- `473 ERR_INVITEONLYCHAN` - Channel is invite-only (+i)
- `474 ERR_BANNEDFROMCHAN` - User is banned (+b)
- `475 ERR_BADCHANNELKEY` - Wrong/missing channel key (+k)

### PART - Leave Channels
**RFC 2812 Section 3.2.2**

```irc
PART <channel>[,<channel>] [:<message>]
```

**Example:**
```irc
PART #general
PART #foo,#bar :Goodbye everyone!
```

### TOPIC - View/Set Channel Topic
**RFC 2812 Section 3.2.4**

```irc
TOPIC <channel>              # View topic
TOPIC <channel> :<new topic> # Set topic (requires +t permission)
TOPIC <channel> :            # Clear topic
```

**Examples:**
```irc
TOPIC #general
TOPIC #general :Welcome to our channel!
TOPIC #general :
```

**Replies:**
- `331 RPL_NOTOPIC` - No topic set
- `332 RPL_TOPIC` - Current topic
- `482 ERR_CHANOPRIVSNEEDED` - Not operator (if +t mode)

### NAMES - List Channel Members
**RFC 2812 Section 3.2.5**

```irc
NAMES [<channel>]
```

**Example:**
```irc
NAMES #general
# Reply: 353 = #general :@alice +bob charlie
#   @ = operator, + = voiced, no prefix = normal user
```

### LIST - List Channels
**RFC 2812 Section 3.2.6**

```irc
LIST [<channel>]
```

Lists all visible channels or specific channel info.

---

## Channel Modes

### MODE - Channel Mode Management
**RFC 2812 Section 3.2.3**

```irc
MODE <channel> [+/-modes] [parameters]
```

### Available Channel Modes

#### +i - Invite Only
Only invited users can join.

```irc
MODE #private +i          # Enable
MODE #private -i          # Disable
```

#### +t - Topic Restricted
Only operators can change topic (default: ON).

```irc
MODE #general +t          # Enable
MODE #general -t          # Disable
```

#### +k - Channel Key (Password)
Requires password to join.

```irc
MODE #secured +k mypassword    # Set key
MODE #secured -k               # Remove key
JOIN #secured mypassword       # Join with key
```

**Special:** Invited users bypass key requirement!

#### +l - User Limit
Limits maximum number of users in channel.

```irc
MODE #limited +l 10       # Set limit to 10 users
MODE #limited -l          # Remove limit
```

**Special:** Invited users bypass limit!

#### +m - Moderated
Only operators (@) and voiced (+v) users can speak.

```irc
MODE #quiet +m            # Enable moderation
MODE #quiet -m            # Disable moderation
```

#### +o - Operator Privilege
Grant/remove operator status.

```irc
MODE #channel +o bob      # Give operator to bob
MODE #channel -o bob      # Remove operator from bob
```

**Operators can:**
- Change channel modes
- Kick users
- Set topic (if +t)
- Invite users (if +i)
- Speak in moderated channels (+m)

#### +v - Voice
Grant/remove voice (ability to speak in moderated channel).

```irc
MODE #channel +v charlie  # Give voice to charlie
MODE #channel -v charlie  # Remove voice from charlie
```

**Voiced users can:**
- Speak in moderated channels (+m)

#### +b - Ban List
Ban users by mask from joining.

```irc
MODE #channel +b *!*@*.evil.com    # Ban mask
MODE #channel +b                   # List all bans
MODE #channel -b *!*@*.evil.com    # Remove ban
```

**Mask Format:** `nick!user@host`
- `*` matches any sequence
- `?` matches single character

**Examples:**
```irc
MODE #channel +b alice!*@*         # Ban nick "alice"
MODE #channel +b *!*@192.168.1.*   # Ban IP range
MODE #channel +b *!baduser@*       # Ban username
```

**Special:** Invited users bypass bans!

#### +e - Exception List
Exception from ban list.

```irc
MODE #channel +e *!*@*.trusted.com  # Add exception
MODE #channel +e                    # List exceptions
MODE #channel -e *!*@*.trusted.com  # Remove exception
```

Users matching exception masks can join even if banned.

### Query Current Modes

```irc
MODE #channel
# Reply: 324 #channel +itkl mykey 50
```

### Mode Combinations

```irc
# Create private, password-protected, moderated channel
MODE #vip +imk secretpass

# Set user limit and give ops
MODE #general +l 100
MODE #general +o bob

# Ban troll but allow friend through exception
MODE #channel +b *!troll@*
MODE #channel +e friend!*@*
```

---

## User Communication

### PRIVMSG - Send Private Message
**RFC 2812 Section 3.3.1**

```irc
PRIVMSG <target> :<message>
```

**Target types:**
- Channel names: `#channel`, `&channel`, `+channel`, `!channel`
- User nicknames

**Examples:**
```irc
PRIVMSG #general :Hello everyone!
PRIVMSG bob :Hey Bob, private message
```

**Moderated Channels:**
In moderated channels (+m), only operators and voiced users can send PRIVMSG.

**Error Replies:**
- `404 ERR_CANNOTSENDTOCHAN` - Cannot send to channel (not member or moderated)
- `401 ERR_NOSUCHNICK` - No such nickname
- `411 ERR_NORECIPIENT` - No recipient specified
- `412 ERR_NOTEXTTOSEND` - No text to send

### NOTICE - Send Notice
**RFC 2812 Section 3.3.2**

```irc
NOTICE <target> :<message>
```

Like PRIVMSG but:
- No automatic replies allowed (prevents loops)
- Errors are silently ignored (no error messages sent back)

**Example:**
```irc
NOTICE #general :Server maintenance in 5 minutes
NOTICE alice :You have been mentioned
```

---

## Channel Management

### KICK - Remove User from Channel
**RFC 2812 Section 3.2.8**

```irc
KICK <channel> <user> [:<reason>]
```

Only operators can kick users.

**Example:**
```irc
KICK #general bob :Spamming
KICK #general bob
```

**Auto-promotion:** If last operator leaves, first remaining member becomes operator.

### INVITE - Invite User to Channel
**RFC 2812 Section 3.2.7**

```irc
INVITE <nickname> <channel>
```

**Invite Privileges:**
Invited users can:
- ✅ Join invite-only channels (+i)
- ✅ Bypass channel key (+k)
- ✅ Bypass user limit (+l)
- ✅ Bypass ban list (+b)

**Example:**
```irc
# Alice invites Bob to private channel
INVITE bob #private

# Bob joins without key or being affected by bans
JOIN #private
```

**Permissions:**
- Any channel member can invite (normal channel)
- Only operators can invite (invite-only +i channel)

**Replies:**
- `341 RPL_INVITING` - Invite sent successfully
- `442 ERR_NOTONCHANNEL` - You're not on that channel
- `443 ERR_USERONCHANNEL` - User already on channel
- `482 ERR_CHANOPRIVSNEEDED` - Need operator in +i channel

---

## Server Information

### CAP - Client Capability Negotiation
**IRCv3 Extension (Optional)**

```irc
CAP LS 302           # List capabilities
CAP REQ :capability  # Request capability
CAP END              # End negotiation
CAP LIST             # List active capabilities
```

**Example:**
```irc
CAP LS 302
# Server: CAP * LS :multi-prefix sasl
CAP END
```

**Note:** CAP is optional - server registration works without it.

### PING/PONG - Keep Alive

```irc
PING :server
PONG :server
```

Server automatically sends PING to check connection health.

### QUIT - Disconnect

```irc
QUIT [:<message>]
```

**Example:**
```irc
QUIT :Goodbye!
```

---

## Testing Examples

### Complete Connection Test

```bash
# Terminal 1: Start server
./ircserv 6667 password

# Terminal 2: Connect with netcat
nc -C localhost 6667
PASS password
NICK alice
USER alice 0 * :Alice Wonderland
JOIN #test
PRIVMSG #test :Hello world!
QUIT :Bye
```

### Test Channel Modes

```irc
# Alice creates secured channel
JOIN #secured
MODE #secured +k mykey
MODE #secured +l 5
MODE #secured +i

# Bob tries to join (fails)
JOIN #secured
# Error: ERR_INVITEONLYCHAN

# Alice invites Bob (Bob bypasses all restrictions)
INVITE bob #secured

# Bob joins without key
JOIN #secured
# Success!
```

### Test Moderated Channel

```irc
# Alice creates moderated channel
JOIN #quiet
MODE #quiet +m

# Bob joins but can't speak
# (alice): JOIN #quiet
PRIVMSG #quiet :test
# Error: ERR_CANNOTSENDTOCHAN

# Alice gives Bob voice
MODE #quiet +v bob

# Now Bob can speak
PRIVMSG #quiet :Thanks for the voice!
# Success!
```

### Test Ban List

```irc
# Operator bans a mask
MODE #channel +b *!*@*.evil.com

# Add exception for friend
MODE #channel +e friend!*@*.evil.com

# List all bans
MODE #channel +b
# Reply: 367 #channel *!*@*.evil.com
#        368 #channel :End of channel ban list

# List all exceptions
MODE #channel +e
# Reply: 348 #channel friend!*@*.evil.com
#        349 #channel :End of channel exception list
```

### Test Multiple Channels with Keys

```irc
# Create three channels with different keys
JOIN #foo
MODE #foo +k key1
JOIN #bar
MODE #bar +k key2
JOIN #baz
MODE #baz +k key3

# Join all three in one command
PART #foo,#bar,#baz
JOIN #foo,#bar,#baz key1,key2,key3
# Success: Joined all three channels!
```

### Test Voice and Moderation

```irc
# Setup moderated channel
JOIN #discussion
MODE #discussion +m

# Give voice to multiple users
MODE #discussion +v bob
MODE #discussion +v charlie

# Only ops and voiced users can speak now
PRIVMSG #discussion :Only we can talk!
```

---

## Implementation Status

### ✅ Fully Implemented Features

**Connection & Registration:**
- ✅ PASS - Password authentication
- ✅ NICK - Nickname management
- ✅ USER - User registration
- ✅ CAP - Capability negotiation (optional)

**Channel Operations:**
- ✅ JOIN - Join channels (multi-channel, dynamic creation, JOIN 0)
- ✅ PART - Leave channels
- ✅ TOPIC - View/set topic
- ✅ NAMES - List members
- ✅ LIST - List channels

**Channel Modes:**
- ✅ +i - Invite only
- ✅ +t - Topic restricted
- ✅ +k - Channel key (password)
- ✅ +l - User limit
- ✅ +m - Moderated
- ✅ +o - Operator privileges
- ✅ +v - Voice
- ✅ +b - Ban list with wildcard masks
- ✅ +e - Exception list

**User Communication:**
- ✅ PRIVMSG - Channel and private messages
- ✅ NOTICE - Notices (no auto-reply)

**Channel Management:**
- ✅ KICK - Remove users
- ✅ INVITE - Invite users (with special privileges)
- ✅ MODE - Channel mode management

**Special Features:**
- ✅ Wildcard pattern matching for bans (* and ?)
- ✅ Invite privileges (bypass key, limit, bans, invite-only)
- ✅ Auto-operator promotion when last op leaves
- ✅ Moderated channel enforcement
- ✅ RFC 2812 numeric replies
- ✅ Proper message formatting
- ✅ User prefix display (@ for ops, + for voice)

---

## Technical Details

### Compilation
```bash
make re
```

### Running
```bash
./ircserv <port> <password>
```

**Example:**
```bash
./ircserv 6667 mypassword
```

### Standards Compliance
- **RFC 2812**: Internet Relay Chat: Client Protocol
- **C++98**: Full C++98 standard compliance
- **Compilation flags**: `-std=c++98 -Wall -Wextra -Werror`

### Architecture
- **Poll-based**: Uses `poll()` for efficient I/O multiplexing
- **Non-blocking**: Handles multiple clients concurrently
- **Dynamic channels**: Channels created automatically on JOIN
- **Persistent state**: Channels persist until empty

---

## Summary

HAI IRC Server implements a complete, RFC 2812-compliant IRC server with:

- ✅ **11+ IRC commands** (PASS, NICK, USER, JOIN, PART, TOPIC, PRIVMSG, NOTICE, MODE, KICK, INVITE, etc.)
- ✅ **9 channel modes** (+i, +t, +k, +l, +m, +o, +v, +b, +e)
- ✅ **Wildcard pattern matching** for ban/exception masks
- ✅ **Invite privilege system** (bypass all restrictions)
- ✅ **Moderated channels** with voice support
- ✅ **Multi-channel operations** with keys
- ✅ **Automatic operator promotion**
- ✅ **RFC 2812 compliant** numerics and message formatting

Perfect for IRC client testing, bot development, and learning IRC protocol implementation!

---

## Quick Reference Card

```
Connection:        PASS <pass>, NICK <nick>, USER <user> 0 * :<real>
Join/Leave:        JOIN #channel [key], PART #channel [:msg]
Communication:     PRIVMSG <target> :<msg>, NOTICE <target> :<msg>
Channel Info:      TOPIC #channel, NAMES #channel, LIST
Management:        KICK #channel <user>, INVITE <user> #channel
Modes (no param):  MODE #channel +/-[itm]
Modes (with user): MODE #channel +/-[ov] <nick>
Modes (with arg):  MODE #channel +/-k <key>, +/-l <limit>
Ban/Exception:     MODE #channel +/-b [mask], +/-e [mask]
Disconnect:        QUIT [:message]
```

---

**Version:** 1.0  
**Last Updated:** December 4, 2025  
**Authors:** HAI IRC Server Team  
**License:** Educational/Academic Use
