/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#include <sys/mman.h>
#include <unistd.h>
#include "file.hh"


void File::open(const char *name, File::flags_type flags) {
	if (fileno!=-1)
		throw std::logic_error("opening an already open File");

	if ((fileno=::open(name, flags))==-1)
		throw system_exception(name);

	name=name;
}


void File::AssertStat() {
	if (!hasstat) {
		if (fstat(fileno, &st)==-1)
			throw system_exception(name);
		hasstat=true;
	}
}


void File::close() {
	if (fileno==-1)
		throw std::logic_error("Closing an already closed File");

	if (::close(fileno)==-1)
		throw system_exception(name);

	fileno=-1;
	hasstat=false;
	name.clear();
}



File::~File() {
	if (fileno!=-1)
		close();
}


void MemoryFile::open(const char *name, File::flags_type flags) {
	if (data)
		throw std::logic_error("opening an already open MemoryFile");

	File fd(name, flags);
	int mapflags = 0;

	if (flags==File::ReadOnly)
		mapflags=PROT_READ;
	else if (flags==File::WriteOnly)
		mapflags=PROT_WRITE;
	else if (flags==File::ReadWrite)
		mapflags=PROT_READ|PROT_WRITE;
	else
		mapflags=PROT_NONE;

	
	size=fd.size();
	data=reinterpret_cast<char*>(mmap(0, fd.size(),
				mapflags, MAP_PRIVATE, fd.fileno, 0));

	if (data==MAP_FAILED)
		throw system_exception();
}


MemoryFile::~MemoryFile() {
	if (data)
		close();

}


void MemoryFile::close() {
	if (!data)
		throw std::logic_error("Closing an already closed MemoryFile");

	if (munmap(data, size)==-1)
		throw system_exception();
}

