/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/27 22:09:16 by hamalmar          #+#    #+#             */
/*   Updated: 2025/09/27 22:09:16 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

int main(int ac, char **av){
    if (ac != 3){
        std::cerr << "Input must be: ./ircserv [port] [password]" << std::endl;
        return (2);
    }
    unsigned int port = 0;
    std::stringstream(av[1]) >> port;
    std::string password(av[2]);
    std::cout << "Port to connect to: " << port << ", the password is: " << password << std::endl;
    return (0);
}
