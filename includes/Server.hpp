/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:04:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/04 17:04:30 by hamalmar         ###   ########.fr       */
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
		/**
			* @brief sockaddr_in is used to describe the Server Address.
			* It has the following fields:
			*
			* @var sin_family  Holds the value of the address family (AF_INET for IPv4).
			* @var sin_port    Holds the port number. Assign using htons(this->port).
			* @var sin_addr    Holds the IP address. sin_addr is a struct with one member:
			*                 in_addr_t s_addr, assigned like this: sin_addr.s_addr = inet_addr("IP Address");
			* @var sin_zero    Padding to make sockaddr_in the same size as sockaddr.
			*
			* @note sin_ is a prefix for Socket Internet.
			-Hamad
		*/
		sockaddr_in serverAddress;
		//This will hold the server socket file descriptor.
		int	serverSocket;
		//This will hold the poll file descriptor.
		int	pollManager;
		//This will hold the server password.
		std::string password;

		pollfd *clients;

		Server();
		Server(const Server& right);
		Server& operator=(const Server&  right);

	public:
		~Server();
		Server(int port, std::string& password);
		void	start(void);
		class InvalidPortNumberException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class PasswordCannotBeEmptyException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class FailedToInitServerSocketException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class FailedToBindServerSocketException: public std::exception{
			public:
				const char	*what() const  throw();
		};

		class FailedToInitPollException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class FailedToListenException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class ReservedPortException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class FailedToInitalizePollFd: public std::exception{
			public:
				const char	*what() const throw();
		};
};

#endif