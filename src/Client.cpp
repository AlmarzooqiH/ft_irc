/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/11 18:46:58 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/11 18:46:58 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Client.hpp"

Client::Client() :
username(""),
nickname(""),
realname(""),
hostname(""),
passwordAuthenticated(false),
nicknameSet(false),
userSet(false)
{}

Client::~Client(){}

Client::Client(const Client& right) :
username(right.username),
nickname(right.nickname),
realname(right.realname),
hostname(right.hostname),
passwordAuthenticated(right.passwordAuthenticated),
nicknameSet(right.nicknameSet),
userSet(right.userSet)
{}

Client& Client::operator=(const Client& right){
	if (this != &right){
		this->username = right.username;
		this->nickname = right.nickname;
		this->realname = right.realname;
		this->hostname = right.hostname;
		this->passwordAuthenticated = right.passwordAuthenticated;
		this->nicknameSet = right.nicknameSet;
		this->userSet = right.userSet;
	}
	return (*this);
}

// Getters
std::string	Client::getNickname(void) const {return (this->nickname);}
std::string	Client::getUsername(void) const {return (this->username);}
std::string	Client::getRealname(void) const {return (this->realname);}
std::string	Client::getHostname(void) const {return (this->hostname);}

// Setters
void	Client::setNickname(const std::string &nNickname) {this->nickname = nNickname;}
void	Client::setUsername(const std::string &nUsername) {this->username = nUsername;}
void	Client::setRealname(const std::string &nRealname) {this->realname = nRealname;}
void	Client::setHostname(const std::string &nHostname) {this->hostname = nHostname;}

// Authentication state setters
void	Client::setPasswordAuthenticated(bool authenticated) {this->passwordAuthenticated = authenticated;}
void	Client::setNicknameSet(bool set) {this->nicknameSet = set;}
void	Client::setUserSet(bool set) {this->userSet = set;}

// Authentication state getters
bool	Client::isPasswordAuthenticated(void) const {return (this->passwordAuthenticated);}
bool	Client::isNicknameSet(void) const {return (this->nicknameSet);}
bool	Client::isUserSet(void) const {return (this->userSet);}

// Check if client has completed full registration (PASS + NICK + USER)
bool	Client::isFullyRegistered(void) const {
	return (this->passwordAuthenticated && this->nicknameSet && this->userSet);
}
