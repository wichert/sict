/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#ifndef __file_hh_included__
#define __file_hh_included__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <boost/noncopyable.hpp>

/** System exception.
 * exception class for standard operating system errors, normally reported
 * using the errno global.
 */
class system_exception : public std::runtime_error {
public:
	/** Default constructor.
	 * This constructor takes the error from errno and uses the
	 * error description from the standard C library.
	 */
	explicit system_exception(int err=errno) : 
		std::runtime_error(std::string(std::strerror(err))),
		error(err) {}

	/** Message constructor.
	 * This constructor takes a message describing the error and the error
	 * value from errno.
	 */
	explicit system_exception(std::string msg, int err=errno) :
		std::runtime_error(msg + ": " + std::strerror(err)),
		error(err) {}

	int error;	/*!< errno value for this exception */
};


/** Simple file wrapper.
 * Use this class to open existing files for reading, writing or both.
 */
class File : public boost::noncopyable {
public:	
	/** File acess types.
	 */
	enum flags_type {
		ReadOnly = O_RDONLY,	/*!< read-only mode */
		WriteOnly = O_WRONLY,	/*!< write-only mode */
		ReadWrite = O_RDWR	/*!< read & write mode */
	};

	/** Default constructor.
	 * Opens a (existing) file.
	 * \param name pathname of file to open
	 * \param flags file access type
	 */
	File(const char *name, flags_type flags = ReadOnly) : hasstat(false), fileno(-1) {
		open(name, flags);
	}

	virtual ~File();

	/** Open a file.
	 * Opens an already existing file. It is not allowed to open a new
	 * file while an already opened.
	 * \param name pathname of file to open
	 * \param flags file access type
	 */
	virtual void open(const char *name, flags_type flags = ReadOnly);


	/** Close a file.
	 * Closes an open file. If closing fails a system_error
	 * exception will be thrown.
	 */
	virtual void close();

	/** Check if a file instance is closed.
	 */
	bool closed() const { return fileno==-1; }

	/** Return the file size.
	 */
	off_t size() { AssertStat(); return st.st_size; }

	int fileno;		/*!< POSIX file descriptor */
	std::string name;	/*!< filename */

protected:
	/** Make sure file stat data has been obtained.
	 * Little helper function to make sure that the stat data is
	 * available.
	 */
	void AssertStat();

	bool hasstat;		/*!< flag indicating if stat data is available */
	struct stat st;		/*!< stat data for this file */
};


/** Memory mapped file.
 * This class allows one to access the content of a file as normally
 * memory. This is done internally by using mmap. No file descriptor
 * is kept open.
 */
class MemoryFile : public boost::noncopyable {
public:
	/** Default constructor.
	 * \param name path of file to read
	 * \param flags desired access type
	 */
	MemoryFile(const char *name, File::flags_type flags=File::ReadOnly) : data(0), size(0) {
		open(name, flags);
	}


	virtual ~MemoryFile();

	/** Open a file.
	 * Opens an already existing file. It is not allowed to open a new
	 * file while an already opened.
	 * \param name pathname of file to open
	 * \param flags file access type
	 */
	void open(const char *name, File::flags_type flags=File::ReadOnly);

	/** Close a file.
	 * Closes an open file. If closing fails a system_error
	 * exception will be thrown.
	 */
	virtual void close();

	char *data;	/*!< pointer to file contents */
	off_t size;	/*!< file size */
};

#endif

