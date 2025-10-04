/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/04 14:28:48 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	#include "SocketHeaders.hpp"

	//Socket related constants
	const std::string SERVER_IP = std::string("127.0.0.1");
	const unsigned int NUMBER_OF_CLIENTS = SOMAXCONN;
	const unsigned int MS_TIMEOUT = 30000;
	const int RESERVED_PORTS = IPPORT_RESERVED;
	const int MAX_PORTS = 65535;

	//Server messages constants
	const std::string INITALIZAE_SERVER = std::string("\033[1;33mAttempting to Initalize the Server\033[0m"); 
	const std::string SERVER_INITALIZED = std::string("\033[1;32mServer has been initalized successfully!\033[0m");
	const std::string SERVER_START_AND_ACCEPT = std::string("\033[1;33mAttempting to start the server and accept connections\033[0m");
	const std::string SERVER_GOODBYE = std::string("\033[1;32mThank you for using HAI Server!\033[0m"); 
#endif