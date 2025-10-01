/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:04:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/01 17:22:40 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Constants.hpp"
# include "SocketHeaders.hpp"
# include "UtilityHeaders.hpp"

class Server{

	private:
		int port;
		sockaddr_in ipvFour;
		int	serverFd;
		int	pollFd;

		Server();
		Server(const Server& right);
		Server& operator=(const Server&  right);

	public:
		~Server();
		Server(int port);
	
		class FailedToInitServerSocketException: public std::exception{
			const char	*what() const throw();
		};
};

#endif