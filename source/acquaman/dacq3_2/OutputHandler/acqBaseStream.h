// Copyright 2008 Canadian Light Source, Inc. All Rights Reserved
//

#if !defined(ACQ_BASE_STREAM_H)
#define ACQ_BASE_STREAM_H 1
#include <string>
#include "acqProperties.h"

class acqBaseOutput;

class acqBaseStream 
{
  public:
	virtual int open() = 0;
	virtual int write(const void *base, unsigned int nbytes) = 0;
	virtual int flush() = 0;
	virtual int offset() = 0;
	virtual int close() = 0;
	virtual int next() = 0;
	virtual bool isReady() = 0;
	virtual std::string &streamName() = 0;

	virtual bool setProperty( std::string propname, std::string propvalue);
	void defProperty( std::string proplist, std::string enumString="");
	std::string &getProperty(std::string propname);
	std::string &getPropertyEnums(std::string propname);
	std::string getPropertyList();
	bool validProperty(std::string propname);
	virtual ~acqBaseStream();
	void setBaseOutput( acqBaseOutput *h) { handler = h; };
	acqBaseOutput * getBaseOutput( ) { return handler; };
	
protected:
	int debug() { return debugFlag; };
	acqBaseStream();
	acqBaseOutput *handler;
	
  private:
	property::properties propertyList;
	unsigned int debugFlag;
}
;


#define PROP_KEEP_OPEN "Keep Open"
#define PROP_STREAM_DEBUG "Debug"
#endif
