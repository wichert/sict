/*
 * Copyright 2004,2005 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */
#include "configdata.hh"

void ConfigData::Merge(const ConfigData &other, bool overwrite, bool typecheck) {
	if (&other==this)
		return;

	if (typecheck && type!=other.type)
		throw typemismatch_error();

	if (overwrite || type==List)
		Clear();

	type=other.type;

	switch (type) {
		case Bogus:
			break;

		case Integer:
			intValue=other.intValue;
			break;

		case String:
			strValue=other.strValue;
			break;

		case List:
			{
			list_type::const_iterator li;

			for (li=other.listValue.begin(); li!=other.listValue.end(); li++)
				listValue.push_back(*li);

			break;
			}

		case Map:
			{
			map_type::const_iterator i;

			for (i=other.mapValue.begin(); i!=other.mapValue.end(); i++)
				try {
					boost::shared_ptr<ConfigData> newvalue(new ConfigData(i->second->type));
					newvalue->Merge(*i->second, overwrite, typecheck);
					mapValue[i->first]=newvalue;
				} catch (typemismatch_error e) {
					e.AddContext(i->first);
					throw e;
				}


			break; }

		default:
			throw std::logic_error("Illegal data type encountered");
	}
}


void ConfigData::Clear() {
	if (type==Map) 
		mapValue.clear();
	else if (type==List)
		listValue.clear();
	else if (type==String)
		strValue.clear();

	type=Bogus;
}
