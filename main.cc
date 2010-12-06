/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#include <iostream>
#include <stdexcept>
#include "tokenize.hh"
#include "iscparser.hh"
#include "file.hh"

boost::shared_ptr<ConfigData> ReadConfig(const char *fn) {
	MemoryFile input(fn);
	Tokenizer toker(input);
	ISCParser parser;

	toker(dynamic_cast<TokenHandler&>(parser));
	return parser.cfg;
}


int main(int argc, char **argv) {
	if (argc!=2) {
		std::cerr << "Wrong number of arguments" << std::endl;
		return 1;
	}

	boost::shared_ptr<ConfigData> defaults;
	boost::shared_ptr<ConfigData> settings;

	try {
		defaults=ReadConfig("defaults");
		settings=ReadConfig("config");
		settings->Merge(*defaults, false, true);
	} catch (EofError) {
		std::cerr << "Unexepcted end of file" << std::endl;
		return 1;
	} catch (parse_error e) {
		std::cerr << "Parse error: " << e.what() << std::endl;
		return 2;
	} catch (error) {
		std::cerr << "Exception caught" << std::endl;
		return 2;
	}

	std::cout << "CGI logdir: " << (const std::string&)(*settings)["CGI"]["logdir"] << std::endl;
	std::cout << "RADIUS port: " << (int)(*settings)["RADIUS"]["server"]["port"] << std::endl;
	
	return 0;
}


