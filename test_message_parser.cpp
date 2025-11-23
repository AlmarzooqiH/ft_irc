/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_message_parser.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/29 00:00:00 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/29 00:00:00 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Message.hpp"
#include <iostream>

void testMessage(const std::string& rawMsg) {
	std::cout << "\n=== Testing: " << rawMsg << " ===" << std::endl;
	
	Message msg(rawMsg);
	
	if (!msg.isValid()) {
		std::cout << "❌ INVALID MESSAGE" << std::endl;
		return;
	}
	
	std::cout << "✅ Valid message parsed successfully" << std::endl;
	
	if (!msg.getPrefix().empty()) {
		std::cout << "Prefix: " << msg.getPrefix() << std::endl;
	}
	
	std::cout << "Command: " << msg.getCommand() << std::endl;
	
	std::vector<std::string> params = msg.getParameters();
	std::cout << "Parameters (" << params.size() << "):" << std::endl;
	for (size_t i = 0; i < params.size(); i++) {
		std::cout << "  [" << i << "] = \"" << params[i] << "\"" << std::endl;
	}
}

int main() {
	std::cout << "IRC Message Parser Test Suite" << std::endl;
	std::cout << "=============================" << std::endl;
	
	// Test 1: Simple command with no parameters
	testMessage("PING");
	
	// Test 2: Command with one parameter
	testMessage("NICK john");
	
	// Test 3: Command with multiple parameters
	testMessage("USER john 0 * realname");
	
	// Test 4: Command with trailing parameter (contains spaces)
	testMessage("USER john 0 * :John Doe");
	
	// Test 5: Message with prefix
	testMessage(":server.com 001 nick :Welcome to IRC");
	
	// Test 6: PRIVMSG with channel and message
	testMessage("PRIVMSG #channel :Hello everyone!");
	
	// Test 7: Server numeric reply
	testMessage(":irc.example.com 372 john :- Message of the day");
	
	// Test 8: CAP command
	testMessage("CAP LS");
	
	// Test 9: Complex message with many parameters
	testMessage("MODE #channel +o john");
	
	// Test 10: Message with prefix from user
	testMessage(":john!~john@host.com PRIVMSG #channel :Hi there");
	
	// Test 11: Invalid - no command
	testMessage(":prefix.only");
	
	// Test 12: PASS command
	testMessage("PASS secret123");
	
	// Test 13: JOIN command
	testMessage("JOIN #channel");
	
	// Test 14: Multiple trailing colons (edge case)
	testMessage("PRIVMSG #test ::) :D :P");
	
	std::cout << "\n=============================" << std::endl;
	std::cout << "Tests completed!" << std::endl;
	
	return 0;
}
