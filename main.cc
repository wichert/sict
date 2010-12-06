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

int main(int argc, char **argv) {
	if (argc!=2) {
		std::cerr << "Wrong number of arguments" << std::endl;
		return 1;
	}

	MemoryFile input(argv[1]);
	Tokenizer toker(input);
	ISCParser parser;

	try {
		toker(dynamic_cast<TokenHandler&>(parser));
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

	std::cout << "CGI logdir: " << (std::string&)(*parser.cfg)["CGI"]["logdir"] << std::endl;
	std::cout << "RADIUS port: " << (int)(*parser.cfg)["RADIUS"]["server"]["port"] << std::endl;
	
	return 0;
}


