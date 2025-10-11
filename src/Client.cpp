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
nickname("")

{}

Client::~Client(){}

Client::Client(const Client& right) :
username(right.username),
nickname(right.nickname)
{}

Client& Client::operator=(const Client& right){
	if (this != &right){
		this->username = right.username;
		this->nickname = right.nickname;
	}
	return (*this);
}

std::string	Client::getNickname(void) const {return (this->nickname);}
std::string	Client::getUsername(void) const {return (this->username);}

void	Client::setNickname(std::string &nNickname) {this->nickname = nNickname;}
void	Client::setUsername(std::string &nUsername) {this->username = nUsername;}
