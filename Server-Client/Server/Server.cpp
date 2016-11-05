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
	//Setup the Address
	SOCKADDR_IN address;
	address.sin_addr.s_addr = inet_addr("192.168.1.100");
	address.sin_family = AF_INET;
	address.sin_port = htons(1111);
	int addressLength = sizeof(address);


	//Listen for Connections
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&address, sizeof(address));
	listen(sListen, SOMAXCONN);

	//Parse HTTP/1.0 Request
	array<string, 3> clientParams;
	char buffer[] = "You are now connected to Dina's Server";

	//Hold Client Connection	
	SOCKET conn;
	conn = accept(sListen, (SOCKADDR*)&address, &addressLength);
	if (conn == 0)
	{
		cout << "Connection Failed" << endl;
	}
	else
	{
		cout << "Client Connected" << endl;
		send(conn, buffer, sizeof(buffer), NULL); //Send Connection Success Message

		char request[4096];
		recv(conn, request, sizeof(request), NULL); //Receive text request from Client
		cout << request << endl;

		clientParams = parseRequest(request);
		string requestCommand;
		string filename;
		string hostname;


		clientParams = parseRequest(request); //Parse request incoming from client [0]GET/POST
		requestCommand = clientParams[0];
		filename = clientParams[1];
		hostname = clientParams[2];

		int exists;
		if (requestCommand == "GET")
		{
			//VALIDATE IF FILE EXISTS
			exists = readFile(filename, buffer);
			if (!exists)
			{
				char buffer[] = "file doesn't exist";
				cout << "file doesn't exist";
				//return 0;
			}
			else
			{
				//READ FILE FROM DISK
				send(conn, buffer, sizeof(buffer), NULL); //Send Connection Success Message
				cout << "File sent succesfully" << endl;
				//CLOSE CONNECTION
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