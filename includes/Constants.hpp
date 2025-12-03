/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/11/26 17:13:28 by ialashqa         ###   ########.fr       */
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
	const std::string MSG_NICKNAME_TAKEN("Nickname is already in use");
	//Client Roles
	const std::string CLIENT_ROLE_REGULAR("Regular");
	const std::string CLIENT_ROLE_OPERATOR("Operator");

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
	const std::string WEECHAT_PING("PING");
	const std::string WEECHAT_PONG("PONG");
	const std::string WEECHAT_JOIN("JOIN");
	const std::string WEECHAT_PRIVMSG("PRIVMSG");
	const std::string WEECHAT_CHANNEL_PREFIX("&#!+"); //According to the protocol manual
	const std::string WEECHAT_TOPIC("TOPIC");
	const std::string WEECHAT_KICK("KICK");
	const std::string WEECHAT_PART("PART");
	const std::string WEECHAT_MODE("MODE");
	const std::string WEECHAT_INVITE("INVITE");

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
		LIST = 1 << 9,
		NICKNAME_TAKEN = 1 << 10,
		PING = 1 << 11,
		JOIN = 1 << 12
	};

	/**
	 * IRC protocol numeric replies (Status codes).
	 * Those code can be found at
	 * https://datatracker.ietf.org/doc/html/rfc2812#section-5
	 * @note RPL_ Stands for Reply.
	 * @note The descriptions i got for each status code are from the
	 * protocol manual. ^^^^^^^
	 * @author Hamad.
	*/
	// Welcome messages
	const std::string RPL_WELCOME("001");
	const std::string RPL_YOURHOST("002");
	const std::string RPL_CREATED("003");
	const std::string RPL_MYINFO("004");
	const std::string RPL_LIST("322");
	const std::string RPL_LISTEND("323");
	const std::string RPL_NOTOPIC("331");
	const std::string RPL_TOPIC("332");
	const std::string RPL_TOPICSET("333");
	const std::string RPL_INVITE("341");

	// Missing/invalid service
	const std::string ERR_NOSUCHSERVICE("408");
	const std::string ERR_NOORIGIN("409");

	// Nickname issues
	const std::string ERR_NICKNAMEINUSE("433");

	// Password issues
	const std::string ERR_PASSWDMISMATCH("464");

	// Other standard errors
	const std::string ERR_NOSUCHNICK("401");
	const std::string ERR_NOSUCHCHANNEL("403");
	const std::string ERR_CANNOTSENDTOCHAN("404");
	const std::string ERR_TOOMANYCHANNELS("405");
	const std::string ERR_UNKNOWNCOMMAND("421");
	const std::string ERR_NONICKNAMEGIVEN("431");
	const std::string ERR_ERRONEUSNICKNAME("432");
	const std::string ERR_USERNOTINCHANNEL("441");
	const std::string ERR_NOTONCHANNEL("442");
	const std::string ERR_USERONCHANNEL("443");
	const std::string ERR_NOTREGISTERED("451");
	const std::string ERR_NEEDMOREPARAMS("461");
	const std::string ERR_ALREADYREGISTRED("462");
	const std::string ERR_CHANNELISFULL("471");
	const std::string ERR_INVITEONLYCHAN("473");
	const std::string ERR_BADCHANNELKEY("475");
	const std::string ERR_BADCHANMASK("476");
	const std::string ERR_CHANOPRIVSNEEDED("482");

	// Hardcoded channels
	const std::string DEFAULT_CHANNEL_1("#general");
	const std::string DEFAULT_CHANNEL_2("#random");
	const std::string DEFAULT_CHANNEL_3("#help");
	const std::string DEFAULT_CHANNEL_4("#announcements");
	const std::string DEFAULT_CHANNEL_5("#bots");

	// If you want an array of them
	const std::string DEFAULT_CHANNELS[] = {
		DEFAULT_CHANNEL_1,
		DEFAULT_CHANNEL_2,
		DEFAULT_CHANNEL_3,
		DEFAULT_CHANNEL_4,
		DEFAULT_CHANNEL_5
	};
	const unsigned int NUM_DEFAULT_CHANNELS = 5;
#endif