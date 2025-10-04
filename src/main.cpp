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

#include "../includes/Irc.hpp"

int main(int ac, char **av){
    if (ac != 3){
        std::cerr << "Input must be: ./ircserv [port] [password]" << std::endl;
        return (2);
    }
    Server *HAIServer;
        /**
         * Why HAI?
         * (H)amad
         * (Abdullah)
         * (I)smail
         * -Hamad
         */
    try{
        std::cout << INITALIZAE_SERVER << std::endl;
        int port = 0;
        std::stringstream(av[1]) >> port;
        std::string password(av[2]);
        HAIServer = new Server(port, password);
    } catch (std::exception& err){
        std::cerr << "\033[1;31m" << err.what() << "\033[0m" << std::endl;
        delete (HAIServer);
        return (2);
    }
    std::cout << SERVER_INITALIZED << std::endl;
    std::cout << SERVER_START_AND_ACCEPT << std::endl;
    try{
        HAIServer->start();
    } catch (std::exception& err){
        std::cerr << "\033[1;31m" << err.what() << "\033[0m" << std::endl;
        delete (HAIServer);
        return (2);
    }
    std::cout << SERVER_GOODBYE << std::endl;
    delete (HAIServer);
    return (0);
}
