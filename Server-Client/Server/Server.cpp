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
	address.sin_addr.s_addr = inet_addr("127.0.0.1"); //specify the address clients connect to
	address.sin_family = AF_INET; //address follows ipv4 
	address.sin_port = htons(1111); //convert port number to network byte order (htons)
	int addressLength = sizeof(address); //used later bind() and accept() system calls


	 /*Message Instances*/
	array<string, 3> clientParams;
	char buffer[] = "You are now connected to Dina's Server"; //Message from server to client.
	char *msg = "";
	char *buffer2;
	char request[4096]; //Receive request messages from client to server
	string requestCommand; //GET or POST
	string filename;
	string hostname; //Server Name
	int exists;
	int length; //get length of file being read.

	 /*Listen for incoming Connections*/
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL); //Specify socket to listen at
	bind(sListen, (SOCKADDR*)&address, addressLength); 
	listen(sListen, SOMAXCONN);


	//Hold Client Connection	
	SOCKET conn;
	fd_set readfds;
	int client_socket[30];
	int max_clients = 30;
	int sd, max_sd;
	for (int i = 0; i < max_clients;i++)
	{
		client_socket[i] = 0;
	}
	while (1)
	{
		FD_ZERO(&readfds);
		FD_SET(sListen,&readfds);
		//max_sd
		conn = accept(sListen, (SOCKADDR*)&address, &addressLength); //Wait for incoming connection from clients
		//select();
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
					char msg[]= "HTTP/1.0 200 OK\r\n";
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
			}
		}

	}
}