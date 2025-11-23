# Manual Testing Guide - IRC Server Commands

## Prerequisites

1. **Start the server**:
   ```bash
   ./ircserv 6667 yourpassword
   ```

2. **Connect with netcat** (in another terminal):
   ```bash
   nc localhost 6667
   ```

---

## Test Case 1: ✅ Successful Registration

**Goal**: Complete full registration and receive welcome messages

**Commands**:
```irc
PASS yourpassword
NICK testuser
USER testuser 0 * :Test User Name
```

**Expected Response**:
```irc
:HAI 001 testuser :Welcome to the HAI IRC Network testuser!testuser@localhost
:HAI 002 testuser :Your host is HAI, running version 1.0
:HAI 003 testuser :This server was created Nov 23 2025
:HAI 004 testuser HAI 1.0 o o
```

**Result**: ✅ You should see all 4 welcome messages

---

## Test Case 2: ❌ Wrong Password

**Goal**: Test password rejection

**Commands**:
```irc
PASS wrongpassword
```

**Expected Response**:
```irc
:HAI 464 * :Password incorrect
```

**Result**: Connection should close immediately

---

## Test Case 3: ⚠️ No Nickname Given

**Goal**: Test error handling for missing parameters

**Commands**:
```irc
PASS yourpassword
NICK
```

**Expected Response**:
```irc
:HAI 431 * :No nickname given
```

**Result**: Error message received, connection stays open

---

## Test Case 4: ❌ Invalid Nickname Format

**Goal**: Test nickname validation

**Commands**:
```irc
PASS yourpassword
NICK 123invalid
```

**Expected Response**:
```irc
:HAI 432 * 123invalid :Erroneous nickname
```

**Try these invalid nicknames**:
- `123abc` (starts with digit)
- `hello world` (contains space)
- `verylongnickname` (> 9 characters)

**Try these valid nicknames**:
- `john`
- `john123`
- `john-doe`
- `[admin]`
- `_bot_`

---

## Test Case 5: ⚠️ Not Enough Parameters for USER

**Goal**: Test parameter validation

**Commands**:
```irc
PASS yourpassword
NICK alice
USER alice
```

**Expected Response**:
```irc
:HAI 461 * USER :Not enough parameters
```

**Correct format**:
```irc
USER alice 0 * :Alice Johnson
```

---

## Test Case 6: ✅ Alternate Order (USER before NICK)

**Goal**: Test that order of NICK/USER doesn't matter

**Commands**:
```irc
PASS yourpassword
USER bob 0 * :Bob Smith
NICK bob
```

**Expected Response** (after NICK command):
```irc
:HAI 001 bob :Welcome to the HAI IRC Network bob!bob@localhost
:HAI 002 bob :Your host is HAI, running version 1.0
:HAI 003 bob :This server was created Nov 23 2025
:HAI 004 bob HAI 1.0 o o
```

**Result**: Welcome messages sent when both NICK and USER are complete

---

## Test Case 7: ❌ Duplicate Nickname

**Goal**: Test nickname uniqueness

**Setup**: 
1. First terminal: Connect and register as "john"
2. Second terminal: Try to register as "john"

**Terminal 1**:
```irc
PASS yourpassword
NICK john
USER john 0 * :John Doe
```

**Terminal 2**:
```irc
PASS yourpassword
NICK john
```

**Expected Response (Terminal 2)**:
```irc
:HAI 433 * john :Nickname is already in use
```

---

## Test Case 8: ❌ Already Registered

**Goal**: Test that you can't re-register

**Commands**:
```irc
PASS yourpassword
NICK charlie
USER charlie 0 * :Charlie
USER charlie 0 * :Charlie Again
```

**Expected Response** (on second USER):
```irc
:HAI 462 charlie :You may not reregister
```

---

## Test Case 9: ❌ PASS After Registration

**Goal**: Test that PASS can't be sent after registration

**Commands**:
```irc
PASS yourpassword
NICK david
USER david 0 * :David
PASS differentpassword
```

**Expected Response** (on second PASS):
```irc
:HAI 462 david :You may not reregister
```

---

## Test Case 10: ⚠️ Command Before Authentication

**Goal**: Test that commands require PASS first

**Commands**:
```irc
NICK eve
```

**Expected Response**:
```irc
:HAI 451 * :You have not registered
```

**Note**: After implementing this check, client must send PASS first

---

## Test Case 11: ✅ CAP Negotiation

**Goal**: Test capability negotiation (optional for clients)

**Commands**:
```irc
CAP LS
PASS yourpassword
NICK frank
USER frank 0 * :Frank
CAP END
```

**Expected Response** (after CAP LS):
```irc
:HAI CAP * LS :
```

---

## Test Case 12: ✅ Valid Nickname Characters

**Goal**: Test various valid nickname formats

**Try these nicknames** (all should work):
```irc
PASS yourpassword
NICK alice
```
```irc
NICK bob123
```
```irc
NICK [admin]
```
```irc
NICK _bot_
```
```irc
NICK john-doe
```
```irc
NICK test_123
```

---

## Troubleshooting

### No Response from Server?
- Check server is running: `ps aux | grep ircserv`
- Check logs: `tail -f server.log`
- Check errors: `tail -f serverError.log`

### Connection Closes Immediately?
- Check password is correct
- Look in `serverError.log` for errors

### Getting "Connection refused"?
- Server not started
- Wrong port number
- Firewall blocking connection

---

## Quick Command Reference

```
PASS <password>          - Authenticate with server
NICK <nickname>          - Set nickname (1-9 chars, letter/special first)
USER <user> 0 * :<real>  - Set username and real name
CAP LS                   - List capabilities
CAP END                  - End capability negotiation
QUIT                     - Disconnect (not implemented yet)
```

---

## Expected Welcome Messages Format

```
:HAI 001 <nick> :Welcome to the HAI IRC Network <nick>!<user>@localhost
:HAI 002 <nick> :Your host is HAI, running version 1.0
:HAI 003 <nick> :This server was created <date>
:HAI 004 <nick> HAI 1.0 o o
```

Where:
- `<nick>` is your nickname
- `<user>` is your username
- `<date>` is server creation date

---

## Tips

✅ **Always start with PASS** - It must be the first command (after optional CAP)

✅ **NICK and USER can be in any order** - But both are required

✅ **Welcome messages appear when registration completes** - After both NICK and USER

✅ **Check server.log** - Shows all received messages

✅ **Check serverError.log** - Shows any errors or issues

---

**Happy Testing! 🧪**
