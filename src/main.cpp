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

// Global pointer for signal handling
static Server* g_serverInstance = NULL;

/**
 * @brief Signal handler for SIGINT and SIGTERM
 * Ensures graceful shutdown and prevents memory leaks
 */
void signalHandler(int signal) {
    (void)signal;
    if (g_serverInstance) {
        std::cout << "\n\033[1;33m[Signal] Shutting down server gracefully...\033[0m" << std::endl;
        g_serverInstance->shutdown();
    }
}

int main(int ac, char **av){
    if (ac != 3){
        std::cerr << "Input must be: ./ircserv [port] [password]" << std::endl;
        return (2);
    }
    Server *HAIServer = NULL;
        /**
         * Why HAI?
         * (H)amad
         * (A)bdullah
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
    
    // Install signal handlers for graceful shutdown
    g_serverInstance = HAIServer;
    signal(SIGINT, signalHandler);   // Handle Ctrl+C
    signal(SIGTERM, signalHandler);  // Handle termination signal
    signal(SIGPIPE, SIG_IGN);        // Ignore broken pipe (client disconnect)
    
    std::cout << SERVER_START_AND_ACCEPT << std::endl;
    try{
        HAIServer->start();
    } catch (std::exception& err){
        std::cerr << "\033[1;31m" << err.what() << "\033[0m" << std::endl;
        g_serverInstance = NULL;
        delete (HAIServer);
        return (2);
    }
    std::cout << SERVER_GOODBYE << std::endl;
    g_serverInstance = NULL;
    delete (HAIServer);
    return (0);
}
