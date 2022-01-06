/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 03/11/2021
 *
 * Description:
 * Creates a client if a valid input IP address and port
 * value is provided.
 * Accepts messages asks the user to enter a message to send
 * to the server.
 */


#include <iostream>
#include <SFML/Network.hpp>
#include <stdlib.h>
#include <conio.h>


void printArgError(std::string arg)
{
/* Purpose: To print out error messages
*  Args:	The argument which caused the error
*  Returns:
*/
	std::cout << "Invalid command line argument detected : " << arg << std::endl;
	std::cout << "Please check your values and press any key to end the program!" << std::endl;
}

int main(int args, char* argv[]) {
	// Get the Ip address
	sf::IpAddress server = argv[1];

	// Check if IP address provided is correct
	if (server == sf::IpAddress::None)
	{
		printArgError(argv[1]);
		char ch = _getch();
		return 0;
	}

	// Check if port is an integer	
	try
	{
		int port = std::stoi(argv[2]);
	}
	catch (int a)
	{
		printArgError(argv[2]);
		char ch = _getch();
		return 0;
	}

	// If port is an integer, check if it is within 61000-65535
	int port = std::stoi(argv[2]);
	if (port < 61000 or port > 65535) 
	{
		printArgError(argv[2]);
		char ch = _getch();
		return 0;
	}

	// Create a socket for commmunicating with the server
	sf::TcpSocket client;

	// Connect to server
	if (client.connect(server, port) != sf::Socket::Done)
	{
		std::cout << "Failed to connect to the server at " << server << " on " << port << "." << std::endl;
		std::cout << "Please check your values and press any key to end the program!" << std::endl;
		char ch = _getch();
		return 0;
	}

	// Continuously prompt the user to input a message
	while (true) 
	{
		std::cout << "Please enter a message: " << std::flush;
		std::string msg;
		std::getline(std::cin, msg);

		// Send the message to the server
		if (client.send(&msg[0], sizeof(msg)) != sf::Socket::Done)
		{
			std::cout << "Failed to send message. Please try again." << std::endl;
		}
	}
	return 0;
}
