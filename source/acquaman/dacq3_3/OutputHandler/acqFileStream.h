#ifndef DACQLIB_ACQFILESTREAM_H
#define DACQLIB_ACQFILESTREAM_H
// Added define statements (David Chevrier, Oct 27 2011)
// Copyright 2008 Canadian Light Source, Inc. All Rights Reserved
//
#include <string>

#include "acqBaseStream.h"

class acqFileStream : acqBaseStream
{
  public:
	// over-rides of base operations
	virtual int write( const void *base, unsigned int nbytes);
	virtual int open();
	virtual int close();
	virtual int next();
	virtual int flush();
	virtual int offset();
	virtual bool isReady();
	virtual std::string &streamName();
	
	virtual bool setProperty( std::string propname, std::string propvalue);

	static acqBaseStream * build_acqFileStream();
	virtual ~acqFileStream();
	
  private:
	acqFileStream();
	
	enum fileType { FT_FILE, FT_DIRECTORY, FT_NONE};

	int buildFileName();
	fileType checkFileType(std::string);
	int autoSeqNo();
	
	std::string fileTemplate;
	std::string filePath;
	std::string outputName;
	unsigned int seqno;
	FILE *fp;
};

extern "C" {
	acqBaseStream *new_acqFileStream();
}


#define acqFileStreamID 20
#define acqFileStream_NewOutput acqOHsignal(1,acqFileStreamID, 1)

///
/// Properties recognized by this handler
///
#define PROP_FILE_TEMPLATE "File Template"
#define PROP_FILE_PATH "File Path"
#define PROP_SEQUENCE_NUMBER "Sequence Number"
#define PROP_OUTPUT_FILE "Output File"

#endif
