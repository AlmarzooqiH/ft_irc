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
		std::string realname;
		std::string hostname;
		
		// Authentication state flags
		bool passwordAuthenticated;
		bool nicknameSet;
		bool userSet;

	public:
		Client();
		Client(const Client& right);
		Client& operator=(const Client& right);
		~Client();

		// Getters
		std::string	getNickname(void) const;
		std::string	getUsername(void) const;
		std::string getRealname(void) const;
		std::string getHostname(void) const;

		// Setters
		void	setNickname(const std::string &nNickname);
		void	setUsername(const std::string &nUsername);
		void	setRealname(const std::string &nRealname);
		void	setHostname(const std::string &nHostname);

		// Authentication state
		void	setPasswordAuthenticated(bool authenticated);
		void	setNicknameSet(bool set);
		void	setUserSet(bool set);

		bool	isPasswordAuthenticated(void) const;
		bool	isNicknameSet(void) const;
		bool	isUserSet(void) const;
		bool	isFullyRegistered(void) const;

};
#endif