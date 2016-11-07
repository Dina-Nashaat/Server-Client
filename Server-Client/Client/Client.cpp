// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
#include "helper.h"
#include "RequestsProvider.h"
#define IPAddress "127.0.0.1"

int main()
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr; //Address to be binded to our Connection socket
	int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function
	addr.sin_addr.s_addr = inet_addr(IPAddress); //Address
	addr.sin_port = htons(1111); //Port = 1111
	addr.sin_family = AF_INET; //IPv4 Socket

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set Connection socket
	vector<string> requests = getRequests();
	array<string, 3> clientParams;
	string requestCommand; //GET or POST
	string filename;
	string hostname; //Server Name

	for (int currentRequest = 0; currentRequest < requests.size(); currentRequest++) {
		if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) //If we are unable to connect...
		{
			MessageBoxA(NULL, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
			continue; //Failed to Connect
		}
		std::cout << "Magdy Connected!" << std::endl;

		//Receive Permission Message from the server
		char permission[256];
		recv(Connection, permission, sizeof(permission), NULL);
		std::cout << "Permission Response: " << permission << std::endl;

		//Current Request to be sent to the server
		char request[1024];
		strncpy_s(request, requests[currentRequest].c_str(), sizeof(request));
		request[sizeof(request) - 1] = 0;
		std::cout << "Sending the Request: " << request << std::endl;

		//Parse the request to be sent
		clientParams = parseRequest(request);
		requestCommand = clientParams[0];
		filename = clientParams[1];
		hostname = clientParams[2];

		//Send the Request
		send(Connection, request, sizeof(request), NULL);

		if (requestCommand == "GET") {
			//Receives response from the server (in case of GET)
			recv(Connection, request, sizeof(request), NULL); 
			std::cout << "Receiving Response: " << request << std::endl;

			//Receives length of file to be read
			char buffer[1024];
			recv(Connection, buffer, sizeof(buffer), NULL);
			cout << "Content-Length: " << buffer << endl;
			int contentLength = atoi(buffer);

			//Recevies the file requested
			char *bufferRecevier = new (nothrow) char[contentLength];
			recv(Connection, bufferRecevier, contentLength, NULL);

			//Write the file to the disk
			char *bufferPointer = bufferRecevier;
			writeFile(filename, bufferPointer, contentLength);
			cout << "File has been received successfully." << endl;
		}
		else if (requestCommand == "POST") {

		}

	}

	system("pause");

}	