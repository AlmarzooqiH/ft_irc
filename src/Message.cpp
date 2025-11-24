/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <aboudi@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 19:52:38 by aboudi          #+#    #+#             */
/*   Updated: 2025/11/24 19:52:45 by aboudi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Message.hpp"

Message::Message() : valid(false) {}

Message::Message(const std::string& rawMsg) : rawMessage(rawMsg), valid(false) {
	parse(rawMsg);
}

Message::Message(const Message& right) {
	*this = right;
}

Message& Message::operator=(const Message& right) {
	if (this != &right) {
		this->prefix = right.prefix;
		this->command = right.command;
		this->parameters = right.parameters;
		this->rawMessage = right.rawMessage;
		this->valid = right.valid;
	}
	return *this;
}

Message::~Message() {}

/**
 * @brief Parse an IRC message according to RFC 2812 format
 * 
 * Message format: [":prefix "] command [params] \r\n
 * params = *14( SPACE middle ) [ SPACE ":" trailing ]
 *        =/ 14( SPACE middle ) [ SPACE [ ":" ] trailing ]
 * 
 * @param rawMsg The raw message string (without \r\n)
 * @return true if parsing succeeded, false otherwise
 */
bool Message::parse(const std::string& rawMsg) {
	clear();
	
	if (rawMsg.empty()) {
		return false;
	}

	this->rawMessage = rawMsg;
	std::string msg = rawMsg;
	size_t pos = 0;

	if (msg[0] == ':') {
		size_t spacePos = msg.find(' ', 1);
		if (spacePos == std::string::npos) {
			return false;
		}
		this->prefix = msg.substr(1, spacePos - 1);
		pos = spacePos + 1;
		while (pos < msg.length() && msg[pos] == ' ') {
			pos++;
		}
	}
	if (pos >= msg.length()) {
		return false;
	}
	size_t cmdEnd = msg.find(' ', pos);
	if (cmdEnd == std::string::npos) {
		this->command = msg.substr(pos);
		this->valid = true;
		return true;
	}

	this->command = msg.substr(pos, cmdEnd - pos);
	pos = cmdEnd + 1;
	while (pos < msg.length() && msg[pos] == ' ') {
		pos++;
	}
	while (pos < msg.length() && this->parameters.size() < 15) {
		if (msg[pos] == ':') {
			this->parameters.push_back(msg.substr(pos + 1));
			break;
		}
		size_t nextSpace = msg.find(' ', pos);
		if (nextSpace == std::string::npos) {
			this->parameters.push_back(msg.substr(pos));
			break;
		}

		this->parameters.push_back(msg.substr(pos, nextSpace - pos));
		pos = nextSpace + 1;
		while (pos < msg.length() && msg[pos] == ' ') {
			pos++;
		}
	}

	this->valid = true;
	return true;
}

void Message::clear() {
	this->prefix.clear();
	this->command.clear();
	this->parameters.clear();
	this->rawMessage.clear();
	this->valid = false;
}

std::string Message::getPrefix() const {
	return this->prefix;
}

std::string Message::getCommand() const {
	return this->command;
}

std::vector<std::string> Message::getParameters() const {
	return this->parameters;
}

std::string Message::getParameter(size_t index) const {
	if (index < this->parameters.size()) {
		return this->parameters[index];
	}
	return "";
}

size_t Message::getParameterCount() const {
	return this->parameters.size();
}

std::string Message::getRawMessage() const {
	return this->rawMessage;
}

bool Message::isValid() const {
	return this->valid;
}