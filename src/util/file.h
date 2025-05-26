#pragma once

#include <stdio.h>
#include "types.h"

// Get the lenght of the file with file pointer, put back cursor to the start
usize	file_lenght_f(FILE *f);
// Get the lenght of the file, put back cursor to the start
usize	file_lenght(const char *path);

// Get the content of a file, exit if file openning fail
// Returned pointer need to be free afterward
char	*file_get(const char *path);

#ifdef UTIL_IMPL

#include "assert.h"
#include "debug.h"

usize	file_lenght_f(FILE *f)
{
	usize	fsize;

	fseek(f, 0, SEEK_END);
	fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	return (fsize);
}

usize	file_lenght(const char *path)
{
	FILE	*f;
	usize	fsize;

	f = fopen(path, "rb");
	ASSERT(f, 
		"Failed to open file '%s'", path);
	
	fsize = file_lenght_f(f);
	fclose(f);
	return (fsize);
}

char	*file_get(const char *path)
{
	FILE	*f;
	char	*fcontent;
	usize	fsize;

	f = fopen(path, "rb");
	ASSERT(f,
		"'%s' - Failed to open file", path);
	
	fsize = file_lenght_f(f);

	fcontent = _malloc((fsize + 1) * sizeof(char));
	ASSERT(fcontent,
		"'%s' - Failed to allocate for file content, trying to allocate %zu bytes.",
		path, fsize);

	usize	read = fread(fcontent, sizeof(char), fsize, f);
	ASSERT(read == fsize,
		" '%s' - Fail when reading file. Expecting %zu chars got %zu",
		path, fsize, read);

	fclose(f);
	fcontent[fsize] = '\0';
	return (fcontent);
}

#endif
