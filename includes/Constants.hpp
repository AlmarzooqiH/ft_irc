/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/11 16:52:56 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	# include "SocketHeaders.hpp"
	# include <string>

	//Socket related constants
	const std::string SERVER_IP("127.0.0.1");
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

	//Weechat constants
	const std::string WEECHAT_PASS("PASS");
	const std::string WEECHAT_CABAILITY("CAP LS");
	const std::string WEECHAT_NICKNAME("NICK");
	const std::string WEECHAT_USER("USER");

	enum WEECHAT_HANDSHAKE{
		PASSWORD = 1 << 0,
		CABAILITY = 1 << 1,
		NICKNAME = 1 << 2,
		USER = 1 << 3,
	};
#endif