// Server.cpp : Defines the entry point for the console application.
//

#include<thread>
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include "stdlib.h"
#pragma comment(lib,"ws2_32.lib")
#include <array>
#include <string>
#include "helper.h"
#include "Windows.h"
#include <process.h>

using namespace std;


char request[4096]; //Receive request messages from client to server
SOCKET conn; //to receive incoming connection from client
unsigned int __stdcall proceedRequest(void *data);
int main()
{
	//WinSock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	
	/*Setup the Address clients will connect to*/
	SOCKADDR_IN address; //Initialize instance of address
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); //specify the address clients connect to
	address.sin_family = AF_INET; //address follows ipv4 
	address.sin_port = htons(1111); //convert port number to network byte order (htons)
	int addressLength = sizeof(address); //used later bind() and accept() system calls


	 /*Message Instances*/
	array<string, 3> clientParams;
	char buffer[] = "You are now connected to Dina's Server"; //Message from server to client.
	char *msg = "";
	char *buffer2;
	
	string requestCommand; //GET or POST
	string filename;
	string hostname; //Server Name
	int exists;
	int length; //get length of file being read.

	 /*Listen for incoming Connections*/
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Specify socket to listen at
	bind(sListen, (SOCKADDR*)&address, addressLength); 
	listen(sListen, SOMAXCONN);

	/*Setup multiple connections*/

	SOCKET client_socket[10]; //Socket to receive incoming connections of multiple clients
	fd_set readfds; //initialize set of socket descriptors
	
	
	int max_clients = 2;  //Maximum number of client-server connections
	int sd, max_sd = 0; //Initialize socket descriptors for each socket connection
	int activity; //get any activity that socket detects

	for (int i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}
	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(sListen, &readfds); //Add listening socket to fd set
		max_sd = sListen;
		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];
			//if valid socket descriptor
			if (sd > 0)
				FD_SET(sd,&readfds);
			//get highest descriptor number for the select statement
			if (sd > max_sd)
				max_sd = sd;
		}
		//select(nfds,);
		struct timeval timeout;
		timeout.tv_sec = 7;
		timeout.tv_usec = 0;
		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

		if (activity < 0 && errno!=EINTR)
			cout<< "Error in select";	

		//If you detect any incoming connections
		if (FD_ISSET(sListen, &readfds))
		{
			conn = accept(sListen, (SOCKADDR*)&address, &addressLength); //Wait for incoming connection from clients
			if (conn == 0)
			{
				cout << "Connection Failed" << endl;
			}
			else
			{
				cout << "Client Connected. Socket fd is " << conn << "ip is " << inet_ntoa(address.sin_addr) << endl;
				send(conn, buffer, sizeof(buffer), NULL); //Send Connection Success Message
				cout << "success message sent to client" << endl;
			}

			for (int i = 0; i < max_clients; i++)
			{
				if (client_socket[i] == 0)
				{
					client_socket[i] = conn;
					cout << "client "<< i << " has been added to socket array" << endl;
					break;
				}
			}
		}

		for (int i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET(sd, &readfds))
			{
				recv(client_socket[i], request, sizeof(request), NULL); //Receive text request from Client
				cout << request << endl; //Print out recieved command. 
				_beginthreadex(0, 0, proceedRequest, NULL, 0, 0);
			}
		}
	//Hold Client Connection	
	}
	
}

unsigned int __stdcall proceedRequest(void *data)
{
	/*Message Instances*/
	array<string, 3> clientParams;
	char buffer[] = "You are now connected to Dina's Server"; //Message from server to client.
	char *msg = "";
	char *buffer2;
	string requestCommand; //GET or POST
	string filename;
	string hostname; //Server Name
	int exists;
	int length; //get length of file being read.

	/*Parse client request (GET or POST)*/
	clientParams = parseRequest(request); //Parse request incoming from client [0]GET/POST
	requestCommand = clientParams[0];
	filename = clientParams[1];
	hostname = clientParams[2];

	if (requestCommand == "GET")
	{
		//VALIDATE IF FILE EXISTS
		exists = readFile(filename, buffer2, &length); //read file returns 1 if file found and inserts into buffer the data of the file. 
		if (!exists)
		{
			char msg[] = "HTTP/1.0 404 Not Found\r\n";
			cout << "File requested by client does not exist";
			send(conn, msg, sizeof(buffer), NULL);
			closesocket(conn);
		}
		else
		{
			//SEND CONFIRMATION MSG
			char msg[] = "HTTP/1.0 200 OK\r\n";
			cout << msg << endl;
			send(conn, msg, sizeof(msg), NULL);

			//READ FILE FROM DISK
			readFile(filename, buffer2, &length);

			//GET FILE LENGTH
			string strLength = to_string(length);
			char buffer3[1024];
			strcpy_s(buffer3, sizeof(buffer), strLength.c_str());
			send(conn, buffer3, sizeof(buffer3), NULL);

			//SEND FILE TO CLIENT
			send(conn, buffer2, length, NULL);

			cout << buffer2; //Print {data data data .... }
			cout << "File sent succesfully" << endl;

			closesocket(conn);
			//client_socket[i] = 0; //initialize for reuse
		}
	}
	else if (requestCommand == "POST")
	{
		char msg[] = "HTTP/1.0 200 OK\r\n";
		send(conn, msg, sizeof(msg), NULL);

		//Receives length of file to be read
		char buffer[1024];
		recv(conn, buffer, sizeof(buffer), NULL);
		cout << "Content-Length: " << buffer << endl;
		int contentLength = atoi(buffer);

		//Recevies the file requested
		char *bufferRecevier = new (nothrow) char[contentLength];
		recv(conn, bufferRecevier, contentLength, NULL);

		//Save the file into disk
		writeFile(filename, bufferRecevier, contentLength);
		cout << "File has been uploaded to server.";
		closesocket(conn);
		//client_socket[i] = 0; //initialize for reuse
	}
	return 0;
}