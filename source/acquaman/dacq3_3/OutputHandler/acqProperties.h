// $Header: OutputHandler/acqProperties.h 1.1 2009/12/18 11:14:37CST Ru Igarashi (igarasr) Exp  $
//
// Copyright 2009 Canadian Light Source, Inc. All Rights Reserved
//
#if !defined ACQ_PROPERTIES_H
#define ACQ_PROPERTIES_H 1
#include <string>
#include <map>

class property {
	std::string value;	// property value
	std::string enumList;	// if non-null, list of property values
public:
	property(std::string myEnums)  : enumList(myEnums) {};
	property()  { } ;
	~property() { } ;
			
	bool hasEnum() { return enumList.length() > 0; };
	std::string &getValue() {return value;};
	bool setValue(std::string v) { value = v; return true; };
	std::string &getEnumList() { return enumList; };
	typedef std::map<std::string,class property*> properties;
	typedef std::map<std::string,class property*>::iterator iterator;
};


#endif
