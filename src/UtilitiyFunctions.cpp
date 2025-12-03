/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UtilitiyFunctions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:42:25 by hamalmar          #+#    #+#             */
/*   Updated: 2025/11/29 20:56:16 by ialashqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/UtilitiyFunctions.hpp"

/**
 * @brief This function will gurantee the message to be sent.
 * @param client The client.
 * @param message The message that we want to send.
 * @note If send() fails we will break from the loop.
 * @return void.
 */
void	sendMessage(pollfd& client, const std::string& message){
	if (client.fd < 0)
		return;
	
	size_t totalBytesSent = 0;
	size_t	messageLength = message.length();

	while (totalBytesSent < messageLength){
		ssize_t sentBytes = send(client.fd, message.c_str() + totalBytesSent, messageLength - totalBytesSent, DEFAULT_FLAG_SEND);
		if (sentBytes <= 0)
			break;
		totalBytesSent += static_cast<size_t>(sentBytes);
	}
	std::cout << "Server sent: " << message;
}
/**
 * @brief This function will gurantee the message to be sent.
 * @param client The client.
 * @param message The message that we want to send.
 * @note If send() fails we will break from the loop.
 * @note This function is going to be used in the Channel class only since Ismail Implementation does not accept pollfd.
 * @return void.
 */
void	channelSendMessage(int clientFd, const std::string& message){
	if (clientFd < 0)
		return;
	
	size_t totalBytesSent = 0;
	size_t	messageLength = message.length();

	while (totalBytesSent < messageLength){
		ssize_t sentBytes = send(clientFd, message.c_str() + totalBytesSent, messageLength - totalBytesSent, DEFAULT_FLAG_SEND);
		if (sentBytes <= 0)
			break;
		totalBytesSent += static_cast<size_t>(sentBytes);
	}
}



/**
 * @brief This function will recieve the data from the client via recv().
 * 
 * @param client The client that wants to send data.
 * @return The message that the client has sent, or empty string on error/disconnect.
 * @author Hamad
 */
std::string	recieveData(pollfd& client){
	char	buffer[BUFFER_SIZE];
	std::memset(buffer, 0, BUFFER_SIZE);
	ssize_t	recievedBytes = recv(client.fd, buffer, BUFFER_SIZE - 1, DEFAULT_FLAG_SEND);
	
	// recv returns 0 when client disconnects, negative on error
	if (recievedBytes <= 0)
		return (std::string(""));
	
	buffer[recievedBytes] = '\0';
	return (std::string(buffer));
}
