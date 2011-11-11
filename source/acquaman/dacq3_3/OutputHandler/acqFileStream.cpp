// Copyright 2008 Canadian Light Source, Inc. All Rights Reserved
//
#include "acqFileStream.h"
#include "acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdlib>
#include <stdarg.h>

#define TRUE 1
#define FALSE 0

#define DEBUG(PTR) if((PTR)->debug())

static acqOutputStreamFactoryRegister myregister("File", acqFileStream::build_acqFileStream);

acqFileStream::acqFileStream() : seqno(1), fp(NULL)
{
	defProperty(PROP_FILE_PATH);
	setProperty(PROP_FILE_PATH, ".");
	defProperty(PROP_FILE_TEMPLATE);
	setProperty(PROP_FILE_TEMPLATE, "acqdata.%03d.dat");
	defProperty(PROP_SEQUENCE_NUMBER);
	defProperty(PROP_OUTPUT_FILE);
}

acqBaseStream*
acqFileStream::build_acqFileStream()
{
	return new acqFileStream();
}

acqBaseStream *
new_acqFileStream()
{
	return acqFileStream::build_acqFileStream();
}


acqFileStream::~acqFileStream()
{
	if( fp && fp != stdout)
		fclose(fp);
	fp = NULL;
}

int acqFileStream:: write( const void *base, unsigned int nbytes)
{
	if( fp)
	{
		 int result = fwrite( base, nbytes, 1, fp);
		 fflush(fp);
		 return result;
	}
	
	return -1;
}

int acqFileStream:: open()
{
	if( fp)
		close();
	
	buildFileName();
	fp = fopen( outputName.c_str(), "a+");
	if( fp == NULL)
	{
		fp = stdout;
		outputName = "< standard output >";
	}
	setProperty( PROP_OUTPUT_FILE, outputName);
	if( getBaseOutput() )
		getBaseOutput()->handlerSignal( acqFileStream_NewOutput, outputName.c_str() );
	
	return 0;
}


int acqFileStream:: offset()
{
	if( fp)
		return ftell(fp);
	return 0;
}

int acqFileStream:: close()
{
	if( fp && fp != stdout)
		fclose(fp);
	fp = NULL;
	setProperty(PROP_OUTPUT_FILE, "");
	return 0;
}

int acqFileStream:: flush()
{
	if( fp)
		return fflush( fp);
	return 0;
}

int acqFileStream:: next()
{
	close();
	open();
	return 0;
	
}

bool acqFileStream:: isReady()
{
	return fp != NULL;
}

std::string &acqFileStream:: streamName()
{
	static std::string name("File");
	return name;
}

///
// build the file names to be used for output
///
int acqFileStream::buildFileName()
{
	std::string namesel;
	fileType ft;

	if( fileTemplate.empty() )
	{
		if( filePath.empty() )
		{
			outputName = "";
			return 0;
		}
		ft = checkFileType(filePath);
		if( ft == FT_FILE)
			namesel = filePath;

		else if( ft == FT_DIRECTORY)
			namesel = filePath + "/";
		else {
			unsigned int slash = filePath.find_last_of('/');
			if( slash <= 0 || slash >= filePath.length() )
			{
				// non-existent directory path
				outputName = "";
				return -1;
			}
			fileTemplate = filePath.substr(slash+1);
			filePath.resize(slash);
		}
	}
	if( namesel.empty() )
	{
		char filename[1024];
		snprintf(filename, sizeof filename, fileTemplate.c_str(), seqno);
		if( ! filePath.empty() )
			namesel = filePath + "/" + filename;
		else
			namesel = filename;
		seqno++;
	}

	DEBUG(this) printf("buildFileName: '%s'\n", namesel.c_str() );
	outputName = namesel;

	return 0;
}

bool
acqFileStream:: setProperty( const std::string name, const std::string value)
{
	acqBaseStream::setProperty(name, value);
	
	if( name == PROP_FILE_TEMPLATE)
	{
		fileTemplate = value;
		autoSeqNo();
	}
	else if( name == PROP_FILE_PATH)
	{
		filePath = value;
		autoSeqNo();
	}
	else if( name == PROP_SEQUENCE_NUMBER)
		seqno = atoi( value.c_str() );
	else if( name == PROP_OUTPUT_FILE)
		outputName = value;
	else
		return 0;
	return 1;
}


acqFileStream::fileType acqFileStream::checkFileType(std::string file)
{
        struct stat buf;
        if( stat(file.c_str(), &buf) < 0)
                return FT_NONE;
        if( S_ISREG(buf.st_mode) )
                return FT_FILE;
        if( S_ISDIR(buf.st_mode) )
                return FT_DIRECTORY;
        /*
         * right now, no support for fifo's or sockets
         */
        return FT_NONE;
}

#include <regex.h>
#include <dirent.h>
/// \brief Find the next available sequence number for the file template.
///
/// based on the existing file path and template, generate the
/// next available sequence number
///
int acqFileStream::autoSeqNo()
{
	std::string pattern = fileTemplate;
	std::string dirname = filePath;

	int slash = pattern.rfind("/");
	if( slash >= 0)
	{
		if( slash > 1)
			dirname += pattern.substr(0,slash-1);
		pattern = pattern.substr(slash+1);
	}
	int idx = pattern.find("%");
	if( idx >= 0)
	{
		int eidx = pattern.find("d", idx);
		DEBUG(this) printf("replacing %s (%d,%d)\n", pattern.c_str(), idx, eidx);
		if( eidx > idx)
			pattern.replace(idx, eidx-idx+1, ".*" );
		else
			return seqno;
		DEBUG(this) printf("...resulting %s\n", pattern.c_str() );
	}

	regex_t reg_pat;
	regcomp(&reg_pat, pattern.c_str(), REG_NOSUB);

	DEBUG(this) printf("...looking in %s\n", dirname.c_str() );
	DIR *dir = opendir( dirname.c_str() );
	if( dir == 0)
		return 0;
	int maxNumber = -1, val, found;
	struct dirent entry, *result;
	while(readdir_r( dir, &entry, &result) >= 0)
	{
		if( result == NULL)
			break;
		found = regexec( &reg_pat, entry.d_name, 0, NULL, 0);
		if( found != 0)
			continue;
		DEBUG(this) printf("...converting '%s'\n", &entry.d_name[idx]);
		val = atoi( &entry.d_name[idx]);
		if( val > maxNumber)
			maxNumber = val;
	}
	regfree( &reg_pat);
	closedir(dir);
	char seqString[100];
	snprintf( seqString, sizeof seqString, "%d", maxNumber+1);
	
	setProperty( PROP_SEQUENCE_NUMBER, seqString);
	
	return maxNumber+1;
}
