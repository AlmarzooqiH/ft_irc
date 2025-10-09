/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/08 23:12:02 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	# include "SocketHeaders.hpp"
	# include <string>

	//Socket related constants
	const std::string SERVER_IP = std::string("127.0.0.1");
	const int NUMBER_OF_CLIENTS = SOMAXCONN;
	const int MS_TIMEOUT = 250;
	const int RESERVED_PORTS = IPPORT_RESERVED;
	const int MAX_PORTS = 65535;

	//Server messages constants
	const std::string INITALIZAE_SERVER = std::string("\033[1;33mAttempting to Initalize the Server\033[0m"); 
	const std::string SERVER_INITALIZED = std::string("\033[1;32mServer has been initalized successfully!\033[0m");
	const std::string SERVER_START_AND_ACCEPT = std::string("\033[1;33mAttempting to start the server and accept connections\033[0m");
	const std::string SERVER_GOODBYE = std::string("\033[1;32mThank you for using HAI Server!\033[0m"); 

	//Exception messages
	const std::string INVALID_PORT = std::string("Invalid Port. Port must be between 0 and 65535");
	const std::string RESERVED_PORT = std::string("Ports between 0 and 1023 are system reserved.");
	
	const std::string EMPTY_PASSWORD = std::string("The password cannot be empty");
	
	const std::string SOCKET_INIT_FAIL = std::string("Failed to initalize the server socket");
	const std::string SOCKET_BIND_FAIL = std::string("Failed to bind the socket to the port");
	const std::string SOCKET_LISTEN_FAIL = std::string("The server has failed to listen to sockets");
	const std::string SOCKET_NONBLOCKING_FAIL = std::string("The server failed to make the socket non blocking");
	const std::string SOCKET_OPTIONS_FAIL = std::string("The server failed to setup options for its socket");
	
	const std::string POLLFD_INIT_FAIL = std::string("The server failed to allocate memorey for pollfd.");

#endif