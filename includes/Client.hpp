/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 18:42:20 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/11 18:42:20 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "UtilityHeaders.hpp"

class Client{
	private:
		std::string username;
		std::string nickname;
		std::string role;
		bool recvNick;
		bool recvUser;
		bool recvCab;

		public:
		Client();
		Client(const Client& right);
		Client& operator=(const Client& right);
		~Client();
		std::string	getNickname(void) const;
		std::string	getUsername(void) const;
		std::string	getRole(void) const;
		bool	getRecvNick(void) const;
		bool	getRecvUser(void) const;
		bool	getRecvCab(void) const;
		
		void	setNickname(std::string &nNickname);
		void	setUsername(std::string &nUsername);
		void	setRole(std::string &nRole);
		void	setRecvNick(bool n);
		void	setRecvUser(bool n);
		void	setRecvCab(bool n);

};
#endif