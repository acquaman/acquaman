#include "acqFactory.h"
#include <vector>

using namespace std;

// Manage the ACQ factory
acqOutputHandlerFactory::acqOutputHandlerFactory()
{
}

acqOutputHandlerFactory::~acqOutputHandlerFactory()
{
}

acqBaseOutput *acqOutputHandlerFactory::new_acqOutput( string handler, string stream)
{
	acqBaseOutput *aop = acqOutputHandlerFactoryRegister::new_acqOutput(handler);
	acqBaseStream *asp = acqOutputStreamFactoryRegister::new_acqStream(stream);
	if( aop && asp)
	{
		aop->setBaseStream(asp);
		asp->setBaseOutput(aop);
	}
	return aop;
}

acqBaseOutput *acqOutputHandlerFactory::new_acqOutput( const string handler)
{
	acqBaseOutput *aop =  acqOutputHandlerFactoryRegister::new_acqOutput(handler);
	if( !aop)
		return NULL;
	
	acqBaseStream *asp = acqOutputStreamFactoryRegister::new_acqStream(aop->default_stream() );
	if( asp)
	{
		aop->setBaseStream(asp);
		asp->setBaseOutput(aop);
	}
	return aop;
}

vector<string> acqOutputHandlerFactory::registeredHandlers()
{
	return acqOutputHandlerFactoryRegister::registeredHandlers();
}

vector<string> acqOutputHandlerFactory::registeredStreams()
{
	return acqOutputStreamFactoryRegister::registeredStreams();
}


// Manage the acq Output Handler Factory
//

acqOutputHandlerFactoryRegister::acqOutputHandlerFactoryRegister(const string name, HandlerCreatorFunction hcf)
{
	factoryRegister *rp;
	rp = find(name);
	if( rp == NULL)
	{
		factoryRegister reg;
		registered().push_back(reg);
		int idx = registered().size()-1;
		registered()[idx].name = name;
		registered()[idx].creator = hcf;
	}
}


acqOutputHandlerFactoryRegister::~acqOutputHandlerFactoryRegister()
{
	// to do: remove the definition (does this happen when unloading a shared library? that would be good!)
}

acqBaseOutput *acqOutputHandlerFactoryRegister::new_acqOutput( const string handler)
{
	factoryRegister *rp;
	rp = find(handler);
	if( rp)
		return rp->creator();
	return NULL;
}

acqOutputHandlerFactoryRegister::factoryRegister * acqOutputHandlerFactoryRegister::find(const string handler)
{
	for(unsigned int i=0; i < registered().size(); i++)
		if(registered()[i].name == handler)
			return &registered()[i];
	return NULL;
}

vector<string> acqOutputHandlerFactoryRegister::registeredHandlers()
{
	vector<string> hl;

	for(unsigned int i =0; i < registered().size(); i++)
		hl.push_back( registered()[i].name);
	return hl;
}


vector<acqOutputHandlerFactoryRegister::factoryRegister> &acqOutputHandlerFactoryRegister::registered()
{
	static vector<factoryRegister> s_registered;
	return s_registered;
}



//
// Manage the ACQ Streams Factory

acqOutputStreamFactoryRegister::acqOutputStreamFactoryRegister(const string name, StreamCreatorFunction hcf)
{
	factoryRegister *rp;
	rp = find(name);
	if( rp == NULL)
	{
		factoryRegister reg;
		registered().push_back(reg);
		int idx = registered().size()-1;
		registered()[idx].name = name;
		registered()[idx].creator = hcf;
	}
}


acqOutputStreamFactoryRegister::~acqOutputStreamFactoryRegister()
{
	// to do: remove the definition (does this happen when unloading a shared library? that would be good!)
}

acqBaseStream *acqOutputStreamFactoryRegister::new_acqStream( const string handler)
{
	factoryRegister *rp;
	rp = find(handler);
	if( rp)
		return rp->creator();
	return NULL;
}

acqOutputStreamFactoryRegister::factoryRegister * acqOutputStreamFactoryRegister::find(const string handler)
{
	for(unsigned int i=0; i < registered().size(); i++)
		if(registered()[i].name == handler)
			return &registered()[i];
	return NULL;
}

vector<string> acqOutputStreamFactoryRegister::registeredStreams()
{
	vector<string> hl;

	for(unsigned int i =0; i <registered().size(); i++)
		hl.push_back( registered()[i].name);
	return hl;
}

vector<acqOutputStreamFactoryRegister::factoryRegister> &acqOutputStreamFactoryRegister::registered()
{
	static vector<factoryRegister> s_registered;
	return s_registered;
}


