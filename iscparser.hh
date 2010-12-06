/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#ifndef _wta_iscparser_included_
#define _wta_iscparser_included_

#include <stdexcept>
#include <stack>
#include <string>
#include <boost/shared_ptr.hpp>
#include "tokenize.hh"
#include "configdata.hh"


/** Parse error exception.
 * Standard parse error exception class, thrown when a parse error is
 * encountered.
 */
class parse_error : public std::runtime_error {
public:
	/** Default constructor.
	 * \param arg string description the error in the parser input
	 */
	explicit parse_error(const std::string& arg) : std::runtime_error(arg) { }
};


/** ISC configuration file parser.
 *
 * This class can parse ISC style configuration files such as used by ISC's
 * bind and DHCP server packages. The format is a hierarchical one allowing
 * for integer and string values as well as lsits of those values.
 */
class ISCParser : public ParsedTokenHandler {
public:
	/** Possible state machine states. */
	typedef enum {
		InSection,		// we started a section but do not know its type yet
		InMap,			// processing a map section, awaiting a new variable or end-of-section
		InMapKeyword,		// processing a map section, got a keyword, awaiting value
		InMapNeedTerminator,	// processing a map section, need a terminator to end variable definition
		InList,			// processing a list section, awaiting a new variable or end-of-section
		InListNeedTerminator,	// processing a list section, need a terminator to end value definition
		EndingSection,		// section ended, waiting for a terminator
	} state_type;

	/** current state of the statemachine. */
	state_type	state;
	/** stack of found tokens that must be processed at a later state. */
	std::stack<std::string> tokenStack;
	/** Stack of configuration contexts in the configuration hierarchy. */
	std::stack<boost::shared_ptr<ConfigData> > contextStack;
	/** the parsed configuration data. */
	boost::shared_ptr<ConfigData> cfg;

	ISCParser();

	virtual void HandleKeyword(std::string data);
	virtual void HandleString(std::string data);
	virtual void HandleInteger(long int data);
	virtual void HandleCharacter(char data);
	virtual void HandleEndOfInput();
	virtual void HandleWhitespace(std::string data);

};

#endif

