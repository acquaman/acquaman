#include "AMConstDbObject.h"

AMConstDbObject::AMConstDbObject(const AMDbObject *object, QObject *parent) :
	QObject(parent)
{
	object_ = object;
}

const AMDbObject* AMConstDbObject::object() const{
	return object_;
}

void AMConstDbObject::setObject(const AMDbObject *object){
	object_ = object;
}
