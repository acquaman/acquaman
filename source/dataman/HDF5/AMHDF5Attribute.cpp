#include "AMHDF5Attribute.h"

AMHDF5Attribute::AMHDF5Attribute(const QString &name, hid_t id_, QObject *parent) : QObject(parent)
{

}

AMHDF5Attribute::~AMHDF5Attribute()
{

}

bool AMHDF5Attribute::exists()
{
	return true;
}

bool AMHDF5Attribute::create()
{
	return true;
}

bool AMHDF5Attribute::open()
{
	return true;
}

bool AMHDF5Attribute::close()
{
	return true;
}

bool AMHDF5Attribute::remove()
{
	return true;
}

bool AMHDF5Attribute::flush()
{
	return true;
}
