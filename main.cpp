#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "src/amanet_client.h"

static std::string to_upper(const char* str) {
	std::string s = str;

	for (char& c : s)
		c = std::toupper(static_cast<unsigned char>(c));

	return s;
}

int main(int argc, char** argv) {
	if (argc != 5 && argc != 6) {
		std::cerr <<
			"Usage: amanet_client <host> <port> <target> <request_verb> [<HTTP version: 1.0 or 1.1(default)>]\n" <<
				"Example:\n" <<
					"				amanet_client www.example.com 80 / get\n" <<
						"			amanet_client www.example.com 80 / get 1.0\n";
		return EXIT_FAILURE;
	}


	if (to_upper(argv[4]) != "GET" &&
			to_upper(argv[4]) != "POST" &&
			to_upper(argv[4]) != "PUT") {
		std::cerr << "Incorrect verb used. Options: get, post, put";
		return EXIT_FAILURE;
	}

	auto const host = argv[1];
	auto const port = argv[2];
	auto const target = argv[3];
	auto const verb = http::string_to_verb(to_upper(argv[4]));
	int version = 11;
	if (argc == 6) {
		if (std::strcmp(argv[5], "1.0") == 0) {
			version = 10;
		} else if (std::strcmp(argv[5], "1.1") != 0) {
			std::cerr << "HTTP version must be 1.0 or 1.1\n";
			return EXIT_FAILURE;
		}
	}

	beast::net::io_context io_ctx;

	std::make_shared<amanet_client>(io_ctx)->run(host, port, target, version, verb);

	io_ctx.run();

	return EXIT_SUCCESS;
}
