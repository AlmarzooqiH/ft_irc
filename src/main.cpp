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
#include <signal.h>

// Global server pointer for signal handler
Server *g_server = NULL;

// Signal handler for Ctrl+C (SIGINT)
void signalHandler(int signum) {
    (void)signum;
    std::cout << "\n\033[1;33m⚠ Shutting down server...\033[0m" << std::endl;
    if (g_server) {
        delete g_server;
        g_server = NULL;
    }
    std::cout << SERVER_GOODBYE << std::endl;
    exit(0);
}

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
        g_server = HAIServer;
    } catch (std::exception& err){
        std::cerr << "\033[1;31m" << err.what() << "\033[0m" << std::endl;
        delete (HAIServer);
        return (2);
    }
    std::cout << SERVER_INITALIZED << std::endl;
    
    signal(SIGINT, signalHandler);
    
    std::cout << SERVER_START_AND_ACCEPT << std::endl;
    std::cout << SERVER_RUNNING << std::endl;
    std::cout << SERVER_READY << std::endl;
    std::cout << "\033[1;36mListening on port " << av[1] << "...\033[0m" << std::endl;
    std::cout << std::endl;
    
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
