#include "AMHDF5File.h"

AMHDF5File::AMHDF5File(const QString &name, QObject *parent)
	: QObject(parent)
{
	name_ = name;
	id_ = -1;
}

AMHDF5File::~AMHDF5File()
{

}

bool AMHDF5File::isHDF5File() const
{
	htri_t returnValue = H5Fis_hdf5(name_.toUtf8().constData());

	if (returnValue > 0)
		return true;

	else if (returnValue == 0)
		return false;

	else{
		// Error stuff.
		return false;
	}
}

