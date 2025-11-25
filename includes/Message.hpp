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


#ifndef MESSAGE_HPP
# define MESSAGE_HPP

# include "UtilityHeaders.hpp"

/**
 * @brief Represents a parsed IRC message according to RFC 2812
 * 
 * IRC Message Format: [":prefix "] command [params] \r\n
 * - Messages are max 512 chars (including \r\n)
 * - Prefix is optional, indicates message origin
 * - Command is either a word or 3-digit numeric
 * - Up to 15 parameters, last can have spaces if prefixed with ':'
 * 
 * @author aboudi
 */
class Message {
	private:
		std::string prefix;                    // Optional message prefix (origin)
		std::string command;                   // IRC command (NICK, JOIN, PRIVMSG, etc.)
		std::vector<std::string> parameters;   // Command parameters (max 15)
		std::string rawMessage;                // Original raw message
		bool valid;                            // Whether parsing succeeded

	public:
		Message();
		Message(const std::string& rawMsg);
		Message(const Message& right);
		Message& operator=(const Message& right);
		~Message();

		// Parsing
		bool parse(const std::string& rawMsg);
		
		// Getters
		std::string getPrefix() const;
		std::string getCommand() const;
		std::vector<std::string> getParameters() const;
		std::string getParameter(size_t index) const;
		size_t getParameterCount() const;
		std::string getRawMessage() const;
		bool isValid() const;

		// Utility
		void clear();
};

#endif