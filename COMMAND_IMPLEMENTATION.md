# IRC Server Command Implementation Guide

## ✅ Implemented Commands

This document describes the fully implemented IRC commands: **PASS**, **NICK**, and **USER**.

---

## 🔐 PASS Command

**Purpose**: Authenticate with the server password (must be first command)

**Syntax**: `PASS <password>`

**Parameters**:
- `<password>`: The server password

**Behavior**:
1. ✅ Must be sent BEFORE NICK and USER
2. ✅ Password is validated against server's password
3. ✅ Wrong password disconnects the client
4. ✅ Cannot be sent after full registration
5. ✅ Marks client as authenticated

**Responses**:
- **Success**: No response (silent success)
- **Error 461**: `ERR_NEEDMOREPARAMS` - Not enough parameters
- **Error 464**: `ERR_PASSWDMISMATCH` - Password incorrect (disconnects)
- **Error 462**: `ERR_ALREADYREGISTRED` - Already registered

**Example**:
```irc
Client → PASS secret123
Server → (silent success if correct)
Server → :HAI 464 * :Password incorrect (if wrong, then disconnect)
```

---

## 👤 NICK Command

**Purpose**: Set or change your nickname

**Syntax**: `NICK <nickname>`

**Parameters**:
- `<nickname>`: Desired nickname (1-9 characters)

**Nickname Rules (RFC 2812)**:
- ✅ First char: letter (A-Z, a-z) or special (`[]\`_^{|}`)
- ✅ Other chars: letter, digit (0-9), special, or dash (-)
- ✅ Maximum 9 characters
- ✅ Must be unique (not in use)

**Behavior**:
1. ✅ Validates nickname format
2. ✅ Checks if nickname is already in use
3. ✅ Can be sent before or after registration
4. ✅ Sends welcome messages if registration completes

**Responses**:
- **Success**: No immediate response (or welcome if registration completes)
- **Error 431**: `ERR_NONICKNAMEGIVEN` - No nickname provided
- **Error 432**: `ERR_ERRONEUSNICKNAME` - Invalid nickname format
- **Error 433**: `ERR_NICKNAMEINUSE` - Nickname already in use

**Registration Check**:
When NICK is set AND USER is set AND PASS is correct → Sends welcome messages (001-004)

**Examples**:
```irc
Client → NICK john
Server → (success, or welcome if this completes registration)

Client → NICK john123
Server → (success)

Client → NICK 123invalid
Server → :HAI 432 * 123invalid :Erroneous nickname

Client → NICK john
Server → :HAI 433 * john :Nickname is already in use
```

---

## 👥 USER Command

**Purpose**: Set username and real name information

**Syntax**: `USER <username> <mode> <unused> <realname>`

**Parameters**:
- `<username>`: Username (no spaces)
- `<mode>`: Mode parameter (usually "0")
- `<unused>`: Unused parameter (usually "*")
- `<realname>`: Real name (can contain spaces - trailing parameter)

**Behavior**:
1. ✅ Must provide all 4 parameters
2. ✅ Cannot be sent twice (error if already set)
3. ✅ Stores username and realname
4. ✅ Sends welcome messages if registration completes

**Responses**:
- **Success**: No immediate response (or welcome if registration completes)
- **Error 461**: `ERR_NEEDMOREPARAMS` - Not enough parameters
- **Error 462**: `ERR_ALREADYREGISTRED` - Already registered

**Registration Check**:
When USER is set AND NICK is set AND PASS is correct → Sends welcome messages (001-004)

**Examples**:
```irc
Client → USER john 0 * :John Doe
Server → (success, or welcome if this completes registration)

Client → USER john
Server → :HAI 461 * USER :Not enough parameters

Client → USER john 0 * :John Doe
Server → :HAI 462 * :You may not reregister (if already sent)
```

---

## 🎯 Registration Flow

### Complete Registration Sequence:

```
┌─────────────────────────────────────────────────────────────┐
│ Step 1: CAP (Optional - capability negotiation)            │
│ Client → CAP LS                                             │
│ Server → :HAI CAP * LS :                                    │
│ Client → CAP END                                            │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 2: PASS (Required - authenticate)                     │
│ Client → PASS secret123                                     │
│ Server → (silent success)                                   │
│ ✅ Client.passwordAuthenticated = true                      │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 3: NICK (Required - set nickname)                     │
│ Client → NICK john                                          │
│ ✅ Client.nicknameSet = true                                │
│ ✅ Client.nickname = "john"                                 │
│                                                              │
│ → Check: If PASS + NICK + USER all set?                    │
│    No: Wait for USER command                                │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ Step 4: USER (Required - set user info)                    │
│ Client → USER john 0 * :John Doe                            │
│ ✅ Client.userSet = true                                    │
│ ✅ Client.username = "john"                                 │
│ ✅ Client.realname = "John Doe"                             │
│                                                              │
│ → Check: If PASS + NICK + USER all set?                    │
│    Yes: SEND WELCOME MESSAGES!                              │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│ Welcome Messages (001-004)                                  │
│ Server → :HAI 001 john :Welcome to the HAI IRC Network...  │
│ Server → :HAI 002 john :Your host is HAI, running v1.0     │
│ Server → :HAI 003 john :This server was created <date>     │
│ Server → :HAI 004 john HAI 1.0 o o                          │
│                                                              │
│ ✅ CLIENT IS NOW FULLY REGISTERED!                          │
└─────────────────────────────────────────────────────────────┘
```

### Order Flexibility:

The server allows **NICK and USER in any order**, as long as PASS comes first:

**Valid Sequences**:
```
✅ PASS → NICK → USER  (most common)
✅ PASS → USER → NICK  (also valid)
✅ CAP LS → CAP END → PASS → NICK → USER
```

**Invalid Sequences**:
```
❌ NICK → PASS → USER  (PASS must be first)
❌ USER → NICK → PASS  (PASS must be first)
❌ PASS → NICK (missing USER - no registration)
❌ PASS → USER (missing NICK - no registration)
```

---

## 📊 Client State Tracking

Each client has the following state flags:

```cpp
class Client {
    bool passwordAuthenticated;  // PASS command received & correct
    bool nicknameSet;           // NICK command received
    bool userSet;               // USER command received
    
    bool isFullyRegistered() {
        return passwordAuthenticated && nicknameSet && userSet;
    }
};
```

### State Transitions:

```
Initial State:
  passwordAuthenticated = false
  nicknameSet = false
  userSet = false

After PASS secret123 (correct):
  passwordAuthenticated = true ✅
  nicknameSet = false
  userSet = false

After NICK john:
  passwordAuthenticated = true
  nicknameSet = true ✅
  userSet = false

After USER john 0 * :John Doe:
  passwordAuthenticated = true
  nicknameSet = true
  userSet = true ✅
  → FULLY REGISTERED! Send welcome messages
```

---

## 🔒 Security Features

### 1. Password Protection
- ✅ PASS command must be sent before NICK/USER
- ✅ Wrong password immediately disconnects client
- ✅ Password required for all clients

### 2. Nickname Validation
- ✅ Format validation (RFC 2812 compliant)
- ✅ Uniqueness check (no duplicate nicknames)
- ✅ Length validation (1-9 characters)

### 3. Registration Protection
- ✅ Cannot send PASS after registration
- ✅ Cannot send USER twice
- ✅ Commands requiring auth check registration status

---

## 🧪 Testing Commands

### Test 1: Successful Registration
```bash
$ nc localhost 6667
PASS yourpassword
NICK testuser
USER testuser 0 * :Test User

Expected Response:
:HAI 001 testuser :Welcome to the HAI IRC Network testuser!testuser@localhost
:HAI 002 testuser :Your host is HAI, running version 1.0
:HAI 003 testuser :This server was created Nov 23 2025
:HAI 004 testuser HAI 1.0 o o
```

### Test 2: Wrong Password
```bash
$ nc localhost 6667
PASS wrongpassword

Expected Response:
:HAI 464 * :Password incorrect
(Connection closes)
```

### Test 3: Invalid Nickname
```bash
$ nc localhost 6667
PASS yourpassword
NICK 123invalid

Expected Response:
:HAI 432 * 123invalid :Erroneous nickname
```

### Test 4: Nickname In Use
```bash
# First client
$ nc localhost 6667
PASS yourpassword
NICK john
USER john 0 * :John Doe

# Second client (in another terminal)
$ nc localhost 6667
PASS yourpassword
NICK john

Expected Response:
:HAI 433 * john :Nickname is already in use
```

### Test 5: Missing Parameters
```bash
$ nc localhost 6667
PASS yourpassword
NICK

Expected Response:
:HAI 431 * :No nickname given
```

### Test 6: Alternate Order (USER before NICK)
```bash
$ nc localhost 6667
PASS yourpassword
USER john 0 * :John Doe
NICK john

Expected Response (after NICK):
:HAI 001 john :Welcome to the HAI IRC Network john!john@localhost
:HAI 002 john :Your host is HAI, running version 1.0
:HAI 003 john :This server was created Nov 23 2025
:HAI 004 john HAI 1.0 o o
```

---

## 📋 Error Code Reference

| Code | Name | Trigger | Message |
|------|------|---------|---------|
| **421** | ERR_UNKNOWNCOMMAND | Unknown command sent | `<command> :Unknown command` |
| **431** | ERR_NONICKNAMEGIVEN | NICK with no parameter | `:No nickname given` |
| **432** | ERR_ERRONEUSNICKNAME | Invalid nickname format | `<nick> :Erroneous nickname` |
| **433** | ERR_NICKNAMEINUSE | Nickname already taken | `<nick> :Nickname is already in use` |
| **451** | ERR_NOTREGISTERED | Command needs registration | `:You have not registered` |
| **461** | ERR_NEEDMOREPARAMS | Missing parameters | `<command> :Not enough parameters` |
| **462** | ERR_ALREADYREGISTRED | Already registered | `:You may not reregister` |
| **464** | ERR_PASSWDMISMATCH | Wrong password | `:Password incorrect` |

---

## 🎓 Implementation Details

### Nickname Validation Algorithm:
```cpp
Valid first char: A-Z, a-z, []\`_^{|}
Valid other chars: A-Z, a-z, 0-9, []\`_^{|}, -
Length: 1-9 characters

Examples:
✅ john
✅ john123
✅ john-doe
✅ [admin]
✅ _bot_

❌ 123john  (starts with digit)
❌ john doe (contains space)
❌ j         (valid, but too short for some uses)
❌ verylongnickname (> 9 chars)
```

### Welcome Message Format (RFC 2812):
```
001: :servername 001 nick :Welcome to the <network> <nick>!<user>@<host>
002: :servername 002 nick :Your host is <servername>, running version <ver>
003: :servername 003 nick :This server was created <date>
004: :servername 004 nick <servername> <version> <usermodes> <chanmodes>
```

---

## ✅ Checklist: What's Implemented

- [x] **PASS Command**
  - [x] Password validation
  - [x] Must be first command
  - [x] Disconnect on wrong password
  - [x] Cannot re-authenticate

- [x] **NICK Command**
  - [x] Format validation (RFC 2812)
  - [x] Uniqueness check
  - [x] Length validation (1-9 chars)
  - [x] Can be used before/after registration

- [x] **USER Command**
  - [x] Parameter validation (4 required)
  - [x] Cannot be sent twice
  - [x] Stores username and realname

- [x] **Client State Management**
  - [x] Track authentication status
  - [x] Track nickname set status
  - [x] Track user set status
  - [x] Full registration detection

- [x] **Welcome Messages**
  - [x] 001 RPL_WELCOME
  - [x] 002 RPL_YOURHOST
  - [x] 003 RPL_CREATED
  - [x] 004 RPL_MYINFO

- [x] **Error Handling**
  - [x] All relevant error codes
  - [x] Proper numeric format
  - [x] Descriptive messages

---

**Your IRC server now has complete PASS, NICK, and USER command implementations! 🎉**
