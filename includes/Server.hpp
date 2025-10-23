/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 17:04:26 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/22 23:09:45 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Constants.hpp"
# include "SocketHeaders.hpp"
# include "UtilityHeaders.hpp"
# include "Client.hpp"

class Server{

	private:
	//This will hold the port number that the user choses.
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
			* @author Hamad
		*/
		sockaddr_in serverAddress;

		//This will hold the server socket file descriptor.
		int	serverSocket;

		//This will hold the number of events by the poll().

		int	pollManager;

		//This will hold the server password.
		std::string password;

		/**
		 * @brief pollfd is going to be used by the @var pollManager where cli-
		 * -ent is gonna have the size of the constant NUMBER_OF_CLIENTS which
		 * is defined in Constants.hpp
		 * 
		 * pollfd is defined as follows according to man 2 poll:
		 *            struct pollfd {
         *      int   fd;         file descriptor
         *      short events;     requested events (What we want from the poll())
         *      short revents;    returned events (What we get from the poll())
         *  };
		 * 
		 * Also check poll.h
		 * 
		 * @author Hamad
		 */
		pollfd *clients;

		//This map will be used to store the client object relative to his file descriptor.
		std::map<int, Client> clientMap;

		//This will hold the buffer of the client when we will be using recv.
		std::map<int, std::string> clientBuffer;

		//This will be used for the event loop.
		bool isRunning;

		/*
			This will hold the number of clients that the server will hold.
			Initally it will hold NUMBER_OF_CLIENTS that is defined in the
			constants header file.
		*/
		size_t	serverCapacity;

		Server();
		Server(const Server& right);
		Server& operator=(const Server&  right);
		void	performHandshake(pollfd& client, const std::string& handshake);
		int	checkHandshake(const std::string& handshake);
		std::string constructHandshake(pollfd& client, int handshakeFlag);
		std::string	recieveData(pollfd& client);
		void	closeClientConnection(pollfd& client);
		void	rejectClient(int clientSocket);
		void	sendMessage(pollfd& client, std::string& message);
		bool	isNicknameTaken(std::string& nickname);

		public:
		~Server();
		Server(int port, const std::string& password);
		void	start(void);

		class InvalidPortNumberException: public std::exception{
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
		class FailedToMakeTheSocketNonBlockingException: public std::exception{
			public:
				const char	*what() const throw();
		};

		class FailedToSetSocketOptionsException: public std::exception{
			public:
				const char	*what() const throw();
		};
};

#endif