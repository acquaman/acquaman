// Copyright 2008 Canadian Light Source, Inc. All Rights Reserved


#if !defined ACQ_FACTORY_H
#define ACQ_FACTORY_H 1

#include "acqBaseStream.h"
#include "acqBaseOutput.h"

#include <string>
#include <vector>

typedef acqBaseOutput * (*HandlerCreatorFunction)();
typedef acqBaseStream * (*StreamCreatorFunction)();

class acqOutputHandlerFactory {
	public:
	static acqBaseOutput * new_acqOutput(const std::string handler, std::string stream);
	static acqBaseOutput * new_acqOutput(const std::string handler);
	static std::vector<std::string> registeredHandlers();
	static std::vector<std::string> registeredStreams();
	
  private:
	acqOutputHandlerFactory();
	~acqOutputHandlerFactory();
};

class acqOutputHandlerFactoryRegister {
	public:
	acqOutputHandlerFactoryRegister(const std::string name, HandlerCreatorFunction hcf);
	~acqOutputHandlerFactoryRegister();
	static acqBaseOutput * new_acqOutput(const std::string handler);
	static std::vector<std::string> registeredHandlers();
	private:
	struct factoryRegister {
		std::string name;
		HandlerCreatorFunction creator;
	};
	static std::vector<factoryRegister> &registered();
	static factoryRegister *find(const std::string name);
};

class acqOutputStreamFactoryRegister {
	public:
	acqOutputStreamFactoryRegister(const std::string name, StreamCreatorFunction hcf);
	~acqOutputStreamFactoryRegister();
	static acqBaseStream * new_acqStream(std::string handler);
	static std::vector<std::string> registeredStreams();
	private:
	struct factoryRegister {
		std::string name;
		StreamCreatorFunction creator;
	};
	static std::vector<factoryRegister> &registered();
	static factoryRegister *find(const std::string name);
};

#endif
