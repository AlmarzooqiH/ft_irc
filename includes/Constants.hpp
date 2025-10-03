/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Constants.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 16:38:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/03 19:35:43 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONSTANTS_HPP
# define CONSTANTS_HPP
	#include "SocketHeaders.hpp"
	const std::string SERVER_IP = std::string("127.0.0.1");
	const unsigned int NUMBER_OF_CLIENTS = SOMAXCONN;
	const unsigned int MS_TIMEOUT = 30000;
	const int RESERVED_PORTS = IPPORT_RESERVED;
	const int MAX_PORTS = 65535;

#endif