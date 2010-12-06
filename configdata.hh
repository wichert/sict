/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#ifndef __wta_configdata_included__
#define __wta_configdata_included__

#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <cassert>

/** Access type errors.
 * An instance of this exception class is thrown you try to cast a ConfigData
 * instance to different type than it is representing. For example trying to
 * extract an integer from an instance containing a string will cause this.
 */
class type_error : public std::logic_error {
public:
	/** Standard constructor.
	 * \param arg Message describing the invalid type usage.
	 */
	explicit type_error(const std::string& arg) : std::logic_error(arg) { }
};


/** Type mismatch error.
 * This exception is used when performing an operation (such as adding,
 * merging or comparing) ConfigData instances and encounntering a type
 * mismatch.
 */
class typemismatch_error : public std::runtime_error {
public:
	/** Standard constructor.
	 * \param ctx Context in which the error occured.
	 */
	explicit typemismatch_error(const std::string& ctx="") : std::runtime_error("Type mismatch"), context(ctx) { }

	/** Add context to the exception.
	 * This function adds a new (parent) scope to the context. This is used
	 * to add context while going back through the stack so the caller can see
	 * where in the configuration space an error occured.
	 *
	 * \param ctx name of context to add
	 */
	void AddContext(const std::string ctx) {
		if (context.empty())
			context=ctx;
		else
			context=ctx + "/" + context;
	}

	virtual ~typemismatch_error() throw() { }
	std::string context; /*!< context in which the error occured. */
};


/** Configuration data container.
 * This class is used to store configuration settings. Configuration
 * data can be of many different types of data (numbers, strings, lists) 
 * and is stored in a hierachical setup.
 *
 * Default values can be implemented easily by using a seperate ConfigData
 * instance for all defaults and merging that into another one with 
 * user supplied data. This can also automatically do the required type
 * checking.
 */
class ConfigData {
public:
	/** Enumeration of possible configuration entry types. */
	enum data_type {
		Bogus,		/*!< entry is bogus and has no value */
		Integer,	/*!< entry contains an integer value */
		String,		/*!< entry contains a string value */
		List,		/*!< entry contains a list of values */
		Map,		/*!< entry contains a configuration section */
	};
	/** Data type used for configuration sections. */ 
	typedef std::map<std::string, boost::shared_ptr<ConfigData> > map_type;
	/** Data type used for lists of values. */
	typedef std::vector<boost::shared_ptr<ConfigData> >	list_type;

	/* We need a default constructur in order to able to use this class
	 * as a value in a map.
	 */
	ConfigData() : type(Bogus) { } 

	/** Valueless constructur.
	 * Simple constructor to create an instance for a specific data type
	 * but without storing a value. In order to store data one of the
	 * specific storage members intValue, strValue, listValue or mapValue
	 * should be used.
	 *
	 * \param dt data type that should be stored in this instance
	 * \sa intValue
	 * \sa strValue
	 * \sa listValue
	 * \sa mapValue
	 */
	explicit ConfigData(data_type dt) : type(dt) { }

	/** Integer constructor.
	 * Construct a new ConfigData instance with an integer value.
	 *
	 * \param data value to store in this configuration entry
	 * \sa intValue
	 */
	explicit ConfigData(int data) : type(Integer), intValue(data) { }

	/** String constructor.
	 * Construct a new ConfigData instance with an string value.
	 *
	 * \param data value to store in this configuration entry
	 * \sa strValue
	 */
	explicit ConfigData(const char *data) : type(String), strValue(data) { }

	/** String constructor.
	 * Construct a new ConfigData instance with an string value.
	 *
	 * \param data value to store in this configuration entry
	 * \sa strValue
	 */
	explicit ConfigData(const std::string &data) : type(String), strValue(data) { }

	/** Standard destructor.
	 *
	 * Nothing special to see here, move along folks.
	 */
	~ConfigData() {
		Clear();
	}

	/** Clear out this bit of configuration space.
	 *
	 * Remove all stored values. This will also reset the type to Bogus.
	 */
	void Clear();

	/*
	template<typename T> T as() const;

	template<int> 
	int as() const {
		if (type!=Integer)
			throw type_error("integer-style access on non-integer data");
		return intValue;
	}

	template<const char*>
	const char* as() const {
		if (type!=String)
			throw type_error("string-style access on non-string data");
		return strValue.c_str();
	}

	template <const std::string&>
	const std::string &as() const {
		if (type!=String)
			throw type_error("string-style access on non-string data");
		return strValue;
	}
*/
	
	/** Integer cast operator.
	 * If a configuration entry stores an integer you can access it
	 * by simply casting it to an integer, which will call this casting
	 * operator. If you try to cast a non-integer to an integer a
	 * type_error exception will be thrown instead.
	 *
	 * \return integer value stored in this entry
	 */
	operator int() const {
		if (type!=Integer)
			throw type_error("integer-style access on non-integer data");
		return intValue;
	}

	/** String cast operator.
	 * If a configuration entry stores a string you can access it
	 * by simply casting it to an string, which will call this casting
	 * operator. If you try to cast a non-string to an string a
	 * type_error exception will be thrown instead.
	 *
	 * \return string value stored in this entry
	 */
	operator const char*() const {
		if (type!=String)
			throw type_error("string-style access on non-string data");
		return strValue.c_str();
	}


	/** String cast operator.
	 * If a configuration entry stores a string you can access it
	 * by simply casting it to an string, which will call this casting
	 * operator. If you try to cast a non-string to an string a
	 * type_error exception will be thrown instead.
	 *
	 * \return string value stored in this entry
	 */
	operator const std::string&() const {
		if (type!=String)
			throw type_error("string-style access on non-string data");
		return strValue;
	}

	/** String cast operator.
	 * If a configuration entry stores a string you can access it
	 * by simply casting it to an string, which will call this casting
	 * operator. If you try to cast a non-string to an string a
	 * type_error exception will be thrown instead.
	 *
	 * \return string value stored in this entry
	 */
	operator std::string&() {
		if (type!=String)
			throw type_error("string-style access on non-string data");
		return strValue;
	}

	/** Array access operator.
	 * If a configuration entry contains a list of values you can easily
	 * access it using this operator. For more specific list access
	 * please use the listValue member directly.
	 *
	 * \return reference to a configuration entry in the list
	 */
	const ConfigData& operator[](int index) const {
		if (type!=List)
			throw type_error("list-style access on non-list data");
		return *listValue[index];
	}

	/** Map access operator.
	 * If a configuration entry contains a new configuration section
	 * one can access it using this operator. For more specific map access
	 * please use the mapValue member directly.
	 *
	 * \return reference to a configuration entry in the subsection
	 */
	const ConfigData& operator[](const char *index) const {
		if (type!=Map)
			throw type_error("list-style access on non-list data");
		const map_type::const_iterator i = mapValue.find(index);
		if (i==mapValue.end())
			throw std::range_error("Key not found");
		return *i->second;
	}


	/** Map access operator.
	 * If a configuration entry contains a new configuration section
	 * one can access it using this operator. For more specific map access
	 * please use the mapValue member directly.
	 *
	 * \return reference to a configuration entry in the subsection
	 */
	const ConfigData& operator[](const std::string &index) const {
		if (type!=Map)
			throw type_error("list-style access on non-list data");
		const map_type::const_iterator i = mapValue.find(index);
		if (i==mapValue.end())
			throw std::range_error("Key not found");
		return *i->second;
	}

	/** Merge config data.
	 * Merge data from another configuration space into this one. There
	 * are two merge methods: overwriting and adding. With the overwrite
	 * method any existing values will be replaced. In adding mode only
	 * non-existing keys will be added. If the typecheck flag is set an
	 * exception will be thrown if a value has a different type in the 
	 * a different type than the original.
	 *
	 * \param other Data to merge into this instance.
	 * \param overwrite overwrite existing values when merging.
	 * \param typecheck insist value types match when overwriting.
	 */
	void Merge(const ConfigData &other, bool overwrite=false, bool typecheck=true);

	/** Merge another configuration space into this one.
	 * This operator merges another configuration space into another
	 * one, overwriting any already existing values.
	 *
	 * \param other configuration space to merge into this instance
	 */
	ConfigData& operator+=(const ConfigData &other) {
		Merge(other, true, true);
		return *this;
	}

	/** Merge another configuration space into this one.
	 * This operator merges another configuration space into another
	 * one without overwriting already existing values.
	 *
	 * \param other configuration space to merge into this instance
	 */
	ConfigData& operator|=(const ConfigData &other) {
		Merge(other, false, true);
		return *this;
	}


	data_type	type;		/*!< data type stored in this entry */
	int		intValue;	/*!< integer value storage */
	std::string	strValue;	/*!< string value storage */
	map_type	mapValue;	/*!< map value storage */
	list_type	listValue;	/*!< list value storage */
};


#endif

