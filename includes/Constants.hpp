/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/22 16:21:21 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	# include "SocketHeaders.hpp"
	# include <string>

	const float SERVER_VERSION = 1.0;

	//Socket related constants
	const std::string SERVER_IP("127.0.0.1");
	const std::string SERVER_NAME("HAI");
	const unsigned int NUMBER_OF_CLIENTS = 16;
	const int MS_TIMEOUT = 250;
	const int RESERVED_PORTS = IPPORT_RESERVED;
	const int MAX_PORTS = 65535;
	const int BUFFER_SIZE = 1024;

	//The CLDR is used to tell the client that this is the end of the message.
	const std::string CLDR("\r\n");

	/**
		This is going to be using in the send() function since
		the socket is already non blocking we dont want to send
		the buffer with any special flags.

		@author Hamad
	*/
	const int DEFAULT_FLAG_SEND = 0;

	/**
		If the user want to become an operator he will have
		to enter this password when executing
		/mode channel +o nickname
	*/
	const std::string OPERATOR_PASSWORD("hehe@1234");

	//Server messages constants
	const std::string INITALIZAE_SERVER("\033[1;33mAttempting to Initalize the Server\033[0m"); 
	const std::string SERVER_INITALIZED("\033[1;32mServer has been initalized successfully!\033[0m");
	const std::string SERVER_START_AND_ACCEPT("\033[1;33mAttempting to start the server and accept connections\033[0m");
	const std::string SERVER_GOODBYE("\033[1;32mThank you for using HAI Server!\033[0m"); 

	//Messages that will be sent to the client.
	const std::string MSG_WELCOME("Welcome to HAI Server!");
	const std::string MSG_SERVER_CREATION("HAI was created today");
	const std::string MSG_SERVER_FULL("Thank you for using HAI server.");
	const std::string MSG_INVALID_PASSWORD("Password is invalid");
	const std::string MSG_SOMETHING_WENT_WRONG("Something went wrong!");

	//Exception messages
	const std::string INVALID_PORT("Invalid Port. Port must be between 0 and 65535");
	const std::string RESERVED_PORT("Ports between 0 and 1023 are system reserved.");
	
	const std::string EMPTY_PASSWORD("The password cannot be empty");
	
	const std::string SOCKET_INIT_FAIL("Failed to initalize the server socket");
	const std::string SOCKET_BIND_FAIL("Failed to bind the socket to the port");
	const std::string SOCKET_LISTEN_FAIL("The server has failed to listen to sockets");
	const std::string SOCKET_NONBLOCKING_FAIL("The server failed to make the socket non blocking");
	const std::string SOCKET_OPTIONS_FAIL("The server failed to setup options for its socket");
	
	const std::string POLLFD_INIT_FAIL("The server failed to allocate memorey for pollfd.");

	//Weechat constants
	const std::string WEECHAT_PASS("PASS");
	const std::string WEECHAT_CABAILITY_LS("CAP LS");
	const std::string WEECHAT_CABAILITY_REQ("CAP REQ");
	const std::string WEECHAT_CABAILITY_END("CAP END");
	const std::string WEECHAT_NICKNAME("NICK");
	const std::string WEECHAT_USER("USER");
	const std::string WEECHAT_QUIT("QUIT");
	const std::string WEECHAT_LIST("LIST");

	enum WEECHAT_HANDSHAKE {
		PASSWORD = 1 << 0,
		INVALID_PASSWORD = 1 << 1,
		CABAILITY_LS = 1 << 2,
		CABAILITY_REQ = 1 << 3,
		CABAILITY_END = 1 << 4,
		NICKNAME = 1 << 5,
		USER = 1 << 6,
		WELCOME_USER = 1 << 7,
		QUIT = 1 << 8,
		LIST = 1 << 9
	};

	/**
		IRC protocol numeric replies (Status codes).
		Those code can be found at
		https://datatracker.ietf.org/doc/html/rfc2812#section-5
		@note RPL_ Stands for Reply.
		@author Hamad.
	*/
	enum STATUS_CODES {
		RPL_WELCOME = 001, //Welcome message.
		RPL_YOURHOST = 002, //Server introduction.
		RPL_CREATED = 003, //Server creation date.
		RPL_MYINFO = 004, //Server version.
		ERR_PASSWDMISMATCH = 464 //Invalid password :P
	};
#endif