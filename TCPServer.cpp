/*
 * Author: Manan Patel
 * Class: ECE 6122
 * Last Date Modified: 03/11/2021
 *
 * Description:
 * Creates a server if a valid input port value is provided.
 * Accepts messages from incoming clients connected on this 
 * server and logs them in a life named "server.log"
 */


#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <conio.h>


class Client
{
/* Purpose: To hold the clients which are connected */
public:
	int num = 0;				// Sr no. of connection
	sf::TcpSocket* socket;		// holds the socket obj ptr
	bool set = false;			// condition used at the end of while loop
};


void printArgError(std::string arg)
{
/* Purpose: To print out error messages
*  Args:	The argument which caused the error
*  Returns:
*/
	std::cout << "Invalid command line argument detected : " << arg << std::endl;
	std::cout << "Please check your values and press any key to end the program!" << std::endl;
}


int main(int argc, char* argv[])
{
	// Open a file if not already exists
	std::ofstream myfile;
	myfile.open("server.log", std::ios::app, std::ios::binary);
	if (!myfile.is_open())
	{
		myfile.open("server.log", std::ios::binary);
	}

	// Check for invalid ports.
	try
	{
		int port = std::stoi(argv[1]);
}
	catch (int a)
	{
		// if port is not an int
		printArgError(argv[1]);
		myfile.close();
		char ch = _getch();
		return 0;
	}

	int port = std::stoi(argv[1]);
	if (port < 61000 or port > 65535)
	{
		// if port is not within 61000-65535
		printArgError(argv[1]);

		// wait for a key press
		char ch = _getch();
		myfile.close();
		return 0;
	}

	int portNum = std::stoi(argv[1]);

	// Listen to the given port for incoming connections
	sf::TcpListener listener;
	listener.setBlocking(false);

	if (listener.listen(portNum) != sf::Socket::Done)
	{
		// server not able to be made on the given port
		std::cout << "Not able to create server. Press any key to end the program." << std::endl;
		myfile.close();

		// wait for a key press
		char ch = _getch();
		myfile.close();
		return 0;
	}
	
	// Server has been created. Now, create a list of Client objects.
	std::vector<Client*> clientList;
	std::size_t received;
	sf::Socket::Status status;

	int clientNo;						
	char msg[1000];						// will store incoming messages
	int	counter = 1;					// will hold srNo of clients

	// continuously check for connections and create a server log
	while (true)
	{
		// Allocate new memory for "potentially" incoming client
		Client* client = new Client();
		sf::TcpSocket* socket = new sf::TcpSocket();
		socket->setBlocking(false);

		// Check to see if any new clients want to connect
		if (listener.accept(*socket) == sf::Socket::Done)
		{
			client->set = true;
			client->socket = socket;
			client->num = counter;
			clientList.push_back(client);

			// Append to file
			myfile << "Client " << client->num << ": Connected." << std::endl;
			counter++;
		}

		// Iterate through to check for incoming messages or disconnections 
		for (int i = 0; i < clientList.size(); i++)
		{
			// Assign values to client object, whose pointer is stored in the vector 
			status = clientList[i]->socket->receive(msg, sizeof(msg), received);
			clientNo = clientList[i]->num;

			if (status == sf::Socket::Done)
			{
				// If any incoming messages
				myfile << "Client " << clientNo << ": " << msg << std::endl;
			}
			else if (status == sf::Socket::Disconnected)
			{
				// If client disconnected
				myfile << "Client " << clientNo << ": Disconnected." << std::endl;			

				// Deallocate memory and then erase from vector
				delete clientList[i]->socket;
				delete clientList[i];
				clientList.erase(clientList.begin() + i);
			}
		}

		// If the memory allocated at the beginning of the loop is unused, deallocate it
		if (client->set == false)
		{
			delete socket;
			delete client;
		}
	}

	myfile.close();
	return 0;
}