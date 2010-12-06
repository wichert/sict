/*
 * Copyright 2004 Wichert Akkerman <wichert@wiggy.net>
 *
 * See COPYING for license information.
 */

#include <sys/mman.h>
#include <unistd.h>
#include "file.hh"


File::File(const char *name, File::flags_type flags) : hasstat(false) {
	if ((fileno=open(name, flags))==-1)
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
	if (::close(fileno)==-1)
		throw system_exception(name);
	fileno=-1;
}



File::~File() {
	close();
}



MemoryFile::MemoryFile(const char *name, File::flags_type flags) {
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
	if (munmap(data, size)==-1)
		throw system_exception();

}

