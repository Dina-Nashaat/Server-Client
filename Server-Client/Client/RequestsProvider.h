#pragma once

#include <array>
#include <iostream>

using namespace std;

vector<string> getRequests() {
	vector<string> requests;

	requests.push_back("GET report.pdf hostname");
	//requests.push_back("GET brush2.gif hostname");

	return requests;
}
