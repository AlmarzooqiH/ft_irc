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
role(""),
recvNick(false),
recvUser(false),
recvCab(false)

{}

Client::~Client(){}

Client::Client(const Client& right) :
username(right.username),
nickname(right.nickname),
role(right.role),
recvNick(right.recvNick),
recvUser(right.recvUser),
recvCab(right.recvCab)
{}

Client& Client::operator=(const Client& right){
	if (this != &right){
		this->username = right.username;
		this->nickname = right.nickname;
		this->role = right.role;
		this->recvNick = right.recvNick;
		this->recvUser = right.recvUser;
		this->recvCab = right.recvCab;
	}
	return (*this);
}

std::string	Client::getNickname(void) const {return (this->nickname);}
std::string	Client::getUsername(void) const {return (this->username);}
std::string	Client::getRole(void) const {return (this->role);}
bool	Client::getRecvNick(void) const {return (this->recvNick);}
bool	Client::getRecvUser(void) const {return (this->recvUser);}
bool	Client::getRecvCab(void) const {return (this->recvCab);}

void	Client::setNickname(const std::string &nNickname) {this->nickname = nNickname;}
void	Client::setUsername(const std::string &nUsername) {this->username = nUsername;}
void	Client::setRole(const std::string &nRole) {this->role = nRole;}
void	Client::setRecvNick(bool nRecvNick) {this->recvNick = nRecvNick;}
void	Client::setRecvUser(bool nRecvUser) {this->recvUser = nRecvUser;}
void	Client::setRecvCab(bool nRecvCab) {this->recvCab = nRecvCab;}
