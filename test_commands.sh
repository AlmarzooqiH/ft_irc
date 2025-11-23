#!/bin/bash

# IRC Server Command Test Script
# Tests PASS, NICK, and USER commands

PORT=6667
PASSWORD="test123"

echo "====================================="
echo "IRC Server Command Tests"
echo "====================================="
echo ""

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test 1: Successful Registration
echo -e "${YELLOW}Test 1: Successful Registration${NC}"
echo "Sending: PASS $PASSWORD, NICK john, USER john 0 * :John Doe"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "NICK john"
  sleep 0.5
  echo "USER john 0 * :John Doe"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 1 completed${NC}\n"

# Test 2: Wrong Password
echo -e "${YELLOW}Test 2: Wrong Password${NC}"
echo "Sending: PASS wrongpassword"
(
  sleep 0.5
  echo "PASS wrongpassword"
  sleep 2
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 2 completed (should disconnect)${NC}\n"

# Test 3: No Nickname Given
echo -e "${YELLOW}Test 3: No Nickname Given${NC}"
echo "Sending: PASS $PASSWORD, NICK (empty)"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "NICK"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 3 completed${NC}\n"

# Test 4: Invalid Nickname Format
echo -e "${YELLOW}Test 4: Invalid Nickname Format${NC}"
echo "Sending: PASS $PASSWORD, NICK 123invalid"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "NICK 123invalid"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 4 completed${NC}\n"

# Test 5: Not Enough USER Parameters
echo -e "${YELLOW}Test 5: Not Enough USER Parameters${NC}"
echo "Sending: PASS $PASSWORD, NICK alice, USER alice"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "NICK alice"
  sleep 0.5
  echo "USER alice"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 5 completed${NC}\n"

# Test 6: Alternate Order (USER before NICK)
echo -e "${YELLOW}Test 6: Alternate Order (USER before NICK)${NC}"
echo "Sending: PASS $PASSWORD, USER bob 0 * :Bob Smith, NICK bob"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "USER bob 0 * :Bob Smith"
  sleep 0.5
  echo "NICK bob"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 6 completed${NC}\n"

# Test 7: Trying to Register Twice
echo -e "${YELLOW}Test 7: Trying to Register Twice${NC}"
echo "Sending: PASS $PASSWORD, NICK charlie, USER charlie 0 * :Charlie, USER charlie 0 * :Charlie Again"
(
  sleep 0.5
  echo "PASS $PASSWORD"
  sleep 0.5
  echo "NICK charlie"
  sleep 0.5
  echo "USER charlie 0 * :Charlie"
  sleep 0.5
  echo "USER charlie 0 * :Charlie Again"
  sleep 2
  echo "QUIT"
) | nc localhost $PORT &
PID=$!
sleep 3
kill $PID 2>/dev/null
echo -e "${GREEN}✓ Test 7 completed${NC}\n"

echo "====================================="
echo -e "${GREEN}All tests completed!${NC}"
echo "Check server.log for detailed output"
echo "====================================="
