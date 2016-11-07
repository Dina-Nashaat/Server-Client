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
	while (getline(ss, token, ' '))
	{
		requestParams[i] = token;
		i++;
	}
	return requestParams;
}

