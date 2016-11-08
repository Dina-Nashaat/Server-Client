#pragma once




#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <array>
#include <fstream>
#define _CRT_SECURE_NO_WARNINGS
using namespace std;


array<string, 3> parseRequest(string request)
{
	array<string, 3> requestParams;
	stringstream ss(request);
	string token;

	int i = 0;
	while (i<3)
	{
		getline(ss, token, ' ');
		requestParams[i] = token;
		i++;
	}
	return requestParams;
}

int getExtension(string filename)
{
	if (filename.substr(filename.find_last_of(".") + 1) == "txt" ||
		filename.substr(filename.find_last_of(".") + 1) == "html")
		return 1;
	else
		return 0;
}

int readFile(string filename, char*&buffer, int*length)
{
	char *readBuffer = "";

	string file_content;
	string str;
	ifstream file(filename);
	int flag = getExtension(filename);

	if (!file.good())
		return 0;
	if (file)
	{
		file.seekg(0, file.end);
		*length = file.tellg();
		file.seekg(0, file.beg);
	}
	file.close();
	if (!flag)
	{
		file.open(filename, fstream::binary);
		cout << "reading in binary" << endl;

		if (file)
		{
			char *readBuffer = new char[*length];
			file.read(readBuffer, *length);
			buffer = readBuffer;
			if (file)
				cout << "Succesfully read" << endl;
			else
				cout << "error: only " << file.gcount() << " could be read" << endl;
			file.close();
		}
	}
	else
	{
		file.open(filename);
		cout << "reading in text mode" << endl;
		while (getline(file, str))
		{
			file_content += str;
			file_content.push_back('\n');
		}
		file_content.push_back('\0');
		char *readBuffer2 = new char[file_content.length() + 1];
		strcpy_s(readBuffer2, sizeof(file_content), file_content.c_str());
		buffer = readBuffer2;
	}
	return 1;
}

void writeFile(string filename, char*&buffer, int length)
{
	// 0 for binary files
	// 1 for text/html files
	int flag = getExtension(filename);
	ofstream outfile;

	if (!flag)
	{
		cout << "writing in binary" << endl;
		outfile.open(filename, fstream::out | fstream::binary);
		outfile.write(buffer, length);
		cout << "File successfully saved." << endl;
		outfile.close();
	}
	else
	{
		cout << "writing in text mode" << endl;
		outfile.open(filename, fstream::out);
		outfile << buffer;
		cout << "File successfully saved." << endl;
		outfile.close();
	}
}