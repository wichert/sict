/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#include <cctype>
#include "tokenize.hh"

void Tokenizer::operator()(TokenHandler &handler) {
	char		bit;
	const char	*start;
	unsigned int	length;

	bit=next();
	while (size) {
		length=1;
		start=input-1;
		if (isdigit(bit)) {
			while (size && isdigit(bit=next()))
				length++;

			handler.HandleInteger(start, length);
		} else if (bit=='"') {
			while ((bit=next())!='"')
				length++;
			bit=next();
			handler.HandleString(start+1, length-1);
		} else if (isspace(bit)) {
			while (size && isspace(bit=next()))
				length++;

			handler.HandleWhitespace(start, length);
		} else  if (isalpha(bit) || bit=='_') {
			while (size && (isalnum(bit=next()) || bit=='_'))
				length++;

			handler.HandleKeyword(start, length);
		} else {
			handler.HandleCharacter(start, length);
			if (size)
				bit=next();
		}
	}

	handler.HandleEndOfInput();
}
