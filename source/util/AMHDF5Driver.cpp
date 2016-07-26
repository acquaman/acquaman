#include "AMHDF5Driver.h"

#include <QDebug>
#include <QVector>

AMHDF5Driver::AMHDF5Driver(QObject *parent)
	: QObject(parent)
{
}

hid_t AMHDF5Driver::createFile()
{
	return H5Fcreate("/Users/hunterd/beamline/programming/hdf5-data/test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
}

hid_t AMHDF5Driver::openFile()
{
	return H5Fopen("/Users/hunterd/beamline/programming/hdf5-data/test.h5", H5F_ACC_RDWR, H5P_DEFAULT);
}

herr_t AMHDF5Driver::closeFile(hid_t fileId)
{
	herr_t error = H5Fclose(fileId);
	qDebug() << "Status was" << error;
	return error;
}

hid_t AMHDF5Driver::addDataSpace()
{
	QVector<hsize_t> dims = QVector<hsize_t>(1, 5);
	return H5Screate_simple(1, dims.constData(), NULL);
}

herr_t AMHDF5Driver::closeDataSpace(hid_t spaceId)
{
	return H5Sclose(spaceId);
}

hid_t AMHDF5Driver::addDataSet(hid_t fileId, hid_t dataSpaceId)
{
	return H5Dcreate2(fileId, "/data", H5T_NATIVE_DOUBLE, dataSpaceId, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
}

hid_t AMHDF5Driver::openDataSet(hid_t fileId)
{
	return H5Dopen2(fileId, "/data", H5P_DEFAULT);
}

herr_t AMHDF5Driver::closeDataSet(hid_t dataSetId)
{
	return H5Dclose(dataSetId);
}

herr_t AMHDF5Driver::readDataSet(hid_t dataSetId, double *data)
{
	return H5Dread(dataSetId, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
}

herr_t AMHDF5Driver::writeDataSet(hid_t dataSetId, double *data)
{
	return H5Dwrite(dataSetId, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
}

hid_t AMHDF5Driver::getDataSpace(hid_t dataSetId)
{
	return H5Dget_space(dataSetId);
}

hid_t AMHDF5Driver::addAttribute(hid_t dataSetId, hid_t dataSpaceId)
{
	return H5Acreate2(dataSetId, "Units", H5T_NATIVE_DOUBLE, dataSpaceId, H5P_DEFAULT, H5P_DEFAULT);
}

herr_t AMHDF5Driver::closeAttribute(hid_t attributeId)
{
	return H5Aclose(attributeId);
}

