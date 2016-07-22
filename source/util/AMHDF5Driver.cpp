#include "AMHDF5Driver.h"

#include <QDebug>

AMHDF5Driver::AMHDF5Driver(QObject *parent)
	: QObject(parent)
{
}

hid_t AMHDF5Driver::openFile()
{
	return H5Fcreate("/Users/hunterd/beamline/programming/hdf5-data/test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
}

herr_t AMHDF5Driver::closeFile(hid_t fileId)
{
	herr_t error = H5Fclose(fileId);
	qDebug() << "Status was" << error;
	return error;
}

