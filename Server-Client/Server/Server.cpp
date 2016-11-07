// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include "stdlib.h"
#pragma comment(lib,"ws2_32.lib")
#include <array>
#include <string>
#include "helper.h"


using namespace std;

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
	address.sin_addr.s_addr = inet_addr("192.168.1.100"); //specify the address clients connect to
	address.sin_family = AF_INET; //address follows ipv4 
	address.sin_port = htons(1111); //convert port number to network byte order (htons)
	int addressLength = sizeof(address); //used later bind() and accept() system calls


	/*Listen for incoming Connections*/
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Specify socket to listen at
	bind(sListen, (SOCKADDR*)&address, addressLength); 
	listen(sListen, SOMAXCONN);

	/*Message Instances*/
	array<string, 3> clientParams;
	char *buffer = "You are now connected to Dina's Server"; //Message from server to client.
	char request[4096]; //Receive request messages from client to server
	string requestCommand; //GET or POST
	string filename;
	string hostname; //Server Name
	int exists;
	int length; //get length of file being read.

	//Hold Client Connection	
	SOCKET conn;
	while (1)
	{
		conn = accept(sListen, (SOCKADDR*)&address, &addressLength); //Wait for incoming connection from clients
		if (conn == 0)
		{
			cout << "Connection Failed" << endl;
		}
		else
		{
			cout << "Client Connected" << endl;
			send(conn, buffer, sizeof(buffer), NULL); //Send Connection Success Message

			
			recv(conn, request, sizeof(request), NULL); //Receive text request from Client
			cout << request << endl; //Print out recieved command. 

			/*Parse client request (GET or POST)*/
			clientParams = parseRequest(request); //Parse request incoming from client [0]GET/POST
			requestCommand = clientParams[0];
			filename = clientParams[1];
			hostname = clientParams[2];

			if (requestCommand == "GET")
			{
				//VALIDATE IF FILE EXISTS
				exists = readFile(filename, buffer,&length); //read file returns 1 if file found and inserts into buffer the data of the file. 
				if (!exists)
				{
					char buffer[] = "HTTP/1.0 404 Not Found \r\n";
					cout << "File requested by client does not exist";
					send(conn, buffer, sizeof(buffer), NULL);
				}
				else
				{
					//READ FILE FROM DISK
					send(conn, buffer, sizeof(buffer), NULL); //Send Connection Success Message
					cout << "File sent succesfully" << endl;
					closesocket(conn);
				}
			}
			else if (requestCommand == "POST")
			{
				recv(conn, request, sizeof(request), NULL);
				//SAVE FILE INTO DISK
				cout << "File has been uploaded to server.";
			}
		}

	}
}