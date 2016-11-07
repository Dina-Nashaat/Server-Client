#pragma once

#include <array>
#include <iostream>

using namespace std;

vector<string> getRequests() {
	vector<string> requests;

	requests.push_back("GET test.gif hostname");

	return requests;
}
