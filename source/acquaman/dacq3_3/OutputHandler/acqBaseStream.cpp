#include "acqBaseStream.h"
#include "acqBaseOutput.h"
#include <cstdlib>


acqBaseStream::acqBaseStream() : handler(NULL), debugFlag(0)
{
	defProperty(PROP_STREAM_DEBUG);
	defProperty(PROP_KEEP_OPEN);
}

static std::string emptyString;

acqBaseStream::~acqBaseStream()
{
}

// set the value for an existing property
bool acqBaseStream::setProperty( std::string propname, std::string propvalue)
{
	if( debugFlag)	std::cout << "setProperty(" << propname << "," << propvalue << ")" << std::endl;
	property::iterator idx = propertyList.find(propname);
	if( idx != propertyList.end())
	{
		idx->second->setValue(propvalue);
		if( propname == PROP_STREAM_DEBUG)
			debugFlag = atoi(propvalue.c_str() );
		acqBaseOutput *ohp = getBaseOutput();
		if( debugFlag) printf("setProperty signalling %p\n", ohp);
		if( ohp)
			ohp->handlerSignal( acqBaseOutput_NewProperty, propname.c_str() );
		return 1;
	}

	// property name not found.
	return 0;
}

void acqBaseStream::defProperty( std::string propname, std::string possibleEnums)
{
	if( !validProperty( propname))
		propertyList[propname] = new property(possibleEnums);
}

// test if propname is in the list
bool
acqBaseStream::validProperty( std::string propname)
{
	return propertyList.find(propname) != propertyList.end();
}

std::string &
acqBaseStream::getProperty(const std::string name)
{
	if(validProperty( name) )
		return propertyList[name]->getValue();
	return emptyString;
}

std::string
acqBaseStream::getPropertyList()
{
	std::string list;
	for( property::iterator idx = propertyList.begin(); idx != propertyList.end(); ++idx)
		list += idx->first + ",";
	return list;
}

std::string &
acqBaseStream::getPropertyEnums(std::string propname)
{
	return propertyList[propname]->getEnumList();
}
