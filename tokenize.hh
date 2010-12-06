/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#ifndef __wta_tokenize_included__
#define __wta_tokenize_included__

#include <string>
#include <cassert>
#include <cstdlib>
#include <cerrno>
#include "file.hh"


/** * Base class for parsing-related errors.
 */
class error {
};


/** End of file exception.
 * This exception is thrown if an end od file os encountered while
 * processing a file.
 */
class EofError : public error {
};


/** Base class for token handlers.
 *
 * This class is half of the parsing framework. The Tokenizer class
 * calls methods from this class for each found token type in its
 * input. This is an abstract base class: derived classes will have to
 * implement all handling functions.
 *
 * \sa Tokenizer
 *
 */
class TokenHandler {
public:
	/** Default constructor. */
	virtual ~TokenHandler() { }

	/** string handler.
	 * This method is called by a Tokenizer instance when a quoted string
	 * is found in the input.
	 *
	 * \param data pointer to found token
	 * \param length length (in bytes) of the token
	 */
	virtual void HandleString(const char *data, unsigned int length) = 0;

	/** integer handler.
	 * This method is called by a Tokenizer instance when an integer
	 * is found in the input.
	 *
	 * \param data pointer to found token
	 * \param length length (in bytes) of the token
	 */
	virtual void HandleInteger(const char *data, unsigned int length) = 0;

	/** keyword  handler.
	 * This method is called by a Tokenizer instance when a keyword
	 * is found in the input. A keyword is a word which is not quoted.
	 *
	 * \param data pointer to found token
	 * \param length length (in bytes) of the token
	 */
	virtual void HandleKeyword(const char *data, unsigned int length) = 0;

	/** character handler.
	 * This method is called by a Tokenizer instance when a character
	 * is found that is not part of a string, integer, keyword and
	 * is not whitespace.
	 *
	 * \param data pointer to found token
	 * \param length length (in bytes) of the token
	 */
	virtual void HandleCharacter(const char *data, unsigned int length) = 0;

	/** whitespace handler.
	 * This method is called by a Tokenizer instance when whitespace is
	 * found in the input. Whitespace that is part of a string will be
	 * handling using the string handler instead.
	 *
	 * \param data pointer to found token
	 * \param length length (in bytes) of the token
	 */
	virtual void HandleWhitespace(const char *data, unsigned int length) = 0;
	/** end of input.
	 * This method is called by a Tokenizer instance when it reaches the
	 * end of its input.
	 */
	virtual void HandleEndOfInput() = 0;
};


/** Parsed token handler.
 *
 * This class is a simple extension to the basic TokenHandler which
 * takes care of converting the raw character data into standard C++
 * datatypes.
 */
class ParsedTokenHandler : public TokenHandler {
	virtual void HandleString(const char *data, unsigned int length) {
		HandleString(std::string(data, 0, length));
	}


	virtual void HandleInteger(const char *data, unsigned int length) {
		char *end;
		long int result;

		result=strtol(data, &end, 0);
		if ((result==LONG_MIN || result==LONG_MAX) && errno==ERANGE)
			throw new error();

		if (end-data>length)
			throw new error(); // internal error really

		HandleInteger(result);
	}


	virtual void HandleKeyword(const char *data, unsigned int length) {
		HandleKeyword(std::string(data, 0, length));
	}


	virtual void HandleCharacter(const char *data, unsigned int length) {
		assert(length==1);
		HandleCharacter(data[0]);
	}


	virtual void HandleWhitespace(const char *data, unsigned int length) {
		HandleWhitespace(std::string(data, 0, length));
	}
	
	/** Handle a string token.
	 * This method is called when a quoted string is found in the input.
	 *
	 * \param data string token
	 */
	virtual void HandleString(std::string data) = 0;

	/** Handle an integer.
	 * This method is called when an integer is found in the input.
	 *
	 * \param data number read from input
	 */
	virtual void HandleInteger(long int data) = 0;

	/** Handle a keyword.
	 * This method is called  when a keyword is found in the input. A
	 * keyword is a word which is not quoted.
	 *
	 * \param data number read from input
	 */
	virtual void HandleKeyword(std::string data) = 0;

	/** Handle a character.
	 * This method is when a character is found that is not part of a
	 * string, integer, keyword and is not whitespace.
	 *
	 * \param data number read from input
	 */
	virtual void HandleCharacter(char data) = 0;

	/** Handle whitespace.
	 * This method is called when whitespace is found in the input.
	 * Whitespace that is part of a string will be handling using the
	 * string handler instead.
	 *
	 * \param data number read from input
	 */
	virtual void HandleWhitespace(std::string data) = 0;
};


/** Tokenizer.
 *
 * This class together with TokenHandler implements a simply parsing framework.
 * A Tokenizer extracts tokens from its input and feeds them to a TokenHandler
 * instance.
 *
 * \sa TokenHandler
 */
class Tokenizer {
public:
	/** File-reading constructor.
	 * This constructor creates a Tokenizer which gets its input from a
	 * MemoryFile instance.
	 *
	 * \param input file to read data from
	 */
	Tokenizer(MemoryFile &input) : input(input.data), size(input.size) { }

	/** Memory-reading constructor.
	 * This constructor creates a Tokenizer which takes its input from
	 * a memory buffer.
	 *
	 * \param data pointer to memory buffer containing data to tokenize
	 * \param length size in bytes of buffer to parse.
	 */
	Tokenizer(const char *data, unsigned int length) : input(data), size(length) { }

	/** Run tokenizing loop.
	 * Calling a Tokenizer instance as a function using this operator
	 * will make it read all tokens from its input and pass them to a
	 * token handler.
	 *
	 * \param handler token handler containing the parser
	 */
	void operator()(TokenHandler &handler);

protected:

	/** Helper function to get the next character.
	 * Gets the next character from input or throw an exception if
	 * the end of input is reached.
	 */
	char next() {
		if (!size)
			throw EofError();
		size--;
		return *input++;
	}


	const char	*input;	/*!< current position in the input stream */
	unsigned int	size;	/*!< remaining size of the input buffer */
};

#endif

