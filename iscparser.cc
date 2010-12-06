/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#include <iostream>
#include "iscparser.hh"
#include "configdata.hh"

ISCParser::ISCParser() : state (InMap), cfg(new ConfigData(ConfigData::Map)) {
	contextStack.push(cfg);
}


void ISCParser::HandleKeyword(std::string data) {
	switch (state) {
		case InSection:
			{
			boost::shared_ptr<ConfigData> newmap(new ConfigData(ConfigData::Map));
			contextStack.top()->mapValue[tokenStack.top()]=newmap;
			contextStack.push(newmap);
			}
			tokenStack.pop();
			// no break here on purpose!

		case InMap:
			tokenStack.push(data);
			state=InMapKeyword;
			break;

		default:
			throw parse_error("keyword not allowed in this context");
	}
}


void ISCParser::HandleString(std::string data) {
	switch (state) {
		case InMapKeyword:
			{
				boost::shared_ptr<ConfigData> newvalue(new ConfigData(data));
				contextStack.top()->mapValue[tokenStack.top()]=newvalue;
			}
			tokenStack.pop();
			state=InMapNeedTerminator;
			break;


		case InSection:
			{
				boost::shared_ptr<ConfigData> newmap(new ConfigData(ConfigData::List));
				contextStack.top()->mapValue[tokenStack.top()]=newmap;
				contextStack.push(newmap);
			}
			tokenStack.pop();
			// no break here on purpose!

		case InList:
			{
				boost::shared_ptr<ConfigData> newvalue(new ConfigData(data));
				contextStack.top()->listValue.push_back(newvalue);
			}
			state=InListNeedTerminator;
			break;

		default:
			throw parse_error("string not allowed in this context");
	}
}


void ISCParser::HandleInteger(long int data) {
	switch (state) {
		case InMapKeyword:
			{
				boost::shared_ptr<ConfigData> newvalue(new ConfigData(data));
				contextStack.top()->mapValue[tokenStack.top()]=newvalue;
			}
			tokenStack.pop();
			state=InMapNeedTerminator;
			break;

		case InList:
			{
				boost::shared_ptr<ConfigData> newvalue(new ConfigData(data));
				contextStack.top()->listValue.push_back(newvalue);
			}
			state=InListNeedTerminator;
			break;

		default:
			throw parse_error("string not allowed in this context");
	}
}


void ISCParser::HandleCharacter(char data) {
	std::string buf;

	if (data=='{')
		switch (state) {
			case InMapKeyword:
				state=InSection;
				break;

			default:
				throw parse_error("Unexpected { found");
		}
	else if (data=='}')
		switch (state) {
			case InSection:
				{
					boost::shared_ptr<ConfigData> newmap(new ConfigData(ConfigData::Map));
					contextStack.top()->mapValue[tokenStack.top()]=newmap;
				}
				tokenStack.pop();
				state=EndingSection;
				break;

			case InMap:
			case InList:
				state=EndingSection;
				break;

			default:
				throw parse_error("Unexpected } found");
		}
	else if (data==';')
		switch (state) {
			case InMapNeedTerminator:
				state=InMap;
				break;

			case InListNeedTerminator:
				state=InList;
				break;

			case EndingSection:
				if (contextStack.size()<=1)
					throw parse_error("Can not close the root section");

				state=InMap;
				contextStack.pop();
				break;

			default:
				throw parse_error("Unexpected seperator (;) found");
		}
	else
		throw parse_error("Unexpected character found");
}


void ISCParser::HandleEndOfInput() {
	if (tokenStack.size() || state!=InMap || contextStack.size()>1)
		throw parse_error("Unexpected end of input");
}


void ISCParser::HandleWhitespace(std::string data) {
}

