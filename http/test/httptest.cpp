#include "HTTP.h"

#include <iostream>

int main()
{
	std::string string("username:password");
	std::cout << "string: " << string;
	std::cout << ", base64: " << HTTP::Base64Encode(string) << std::endl;
	return 0;
}
