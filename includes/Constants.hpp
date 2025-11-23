/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/11 18:35:05 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	# include "SocketHeaders.hpp"
	# include <string>

	//Socket related constants
	const std::string SERVER_IP("127.0.0.1");
	const std::string SERVER_NAME("HAI");
	const int NUMBER_OF_CLIENTS = 256;
	const int MS_TIMEOUT = 250;
	const int RESERVED_PORTS = IPPORT_RESERVED;
	const int MAX_PORTS = 65535;
	const int BUFFER_SIZE = 1024;

	//Server messages constants
	const std::string INITALIZAE_SERVER("\033[1;33mAttempting to Initalize the Server\033[0m"); 
	const std::string SERVER_INITALIZED("\033[1;32mServer has been initalized successfully!\033[0m");
	const std::string SERVER_START_AND_ACCEPT("\033[1;33mAttempting to start the server and accept connections\033[0m");
	const std::string SERVER_GOODBYE("\033[1;32mThank you for using HAI Server!\033[0m"); 

	//Client messages constants
	const std::string CLIENT_CONNECTED("Welcome to HAI Server!\n");
	const std::string CLIENT_INVALID_PASSWORD("Password is invalid");
	const std::string CLIENT_SOMETHING_WENT_WRONG("Something went wrong!");

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

	//Servre CAPBALITIES
	const std::string SERVER_CABAILITY("CAP * LS :\r\n");

	//Weechat constants
	const std::string WEECHAT_PASS("PASS");
	const std::string WEECHAT_CABAILITY_LS("CAP LS");
	const std::string WEECHAT_CABAILITY_REQ("CAP REQ");
	const std::string WEECHAT_CABAILITY_END("CAP END");
	const std::string WEECHAT_NICKNAME("NICK");
	const std::string WEECHAT_USER("USER");

	enum WEECHAT_HANDSHAKE{
		PASSWORD = 1 << 0,
		INVALID_PASSWORD = 1 << 1,
		CABAILITY_LS = 1 << 2,
		CABAILITY_REQ = 1 << 3,
		CABAILITY_END = 1 << 4,
		NICKNAME = 1 << 5,
		USER = 1 << 6,
	};

	// IRC Numeric Replies (RFC 2812)
	// Welcome messages (001-004)
	const std::string RPL_WELCOME("001");
	const std::string RPL_YOURHOST("002");
	const std::string RPL_CREATED("003");
	const std::string RPL_MYINFO("004");

	// Error replies
	const std::string ERR_NOSUCHNICK("401");
	const std::string ERR_NOSUCHCHANNEL("403");
	const std::string ERR_CANNOTSENDTOCHAN("404");
	const std::string ERR_TOOMANYCHANNELS("405");
	const std::string ERR_UNKNOWNCOMMAND("421");
	const std::string ERR_NONICKNAMEGIVEN("431");
	const std::string ERR_ERRONEUSNICKNAME("432");
	const std::string ERR_NICKNAMEINUSE("433");
	const std::string ERR_USERNOTINCHANNEL("441");
	const std::string ERR_NOTONCHANNEL("442");
	const std::string ERR_NOTREGISTERED("451");
	const std::string ERR_NEEDMOREPARAMS("461");
	const std::string ERR_ALREADYREGISTRED("462");
	const std::string ERR_PASSWDMISMATCH("464");
	const std::string ERR_CHANNELISFULL("471");
	const std::string ERR_INVITEONLYCHAN("473");
	const std::string ERR_BADCHANNELKEY("475");
	const std::string ERR_BADCHANMASK("476");
	const std::string ERR_CHANOPRIVSNEEDED("482");

#endif