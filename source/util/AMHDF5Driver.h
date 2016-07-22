#ifndef AMHDF5DRIVER_H
#define AMHDF5DRIVER_H

#include <QObject>

#include "hdf5.h"

class AMHDF5Driver : public QObject
{
	Q_OBJECT

public:
	explicit AMHDF5Driver(QObject *parent = 0);

	hid_t createFile();
	hid_t openFile();
	herr_t closeFile(hid_t fileId);
	hid_t addDataSpace();
	herr_t closeDataSpace(hid_t spaceId);
	hid_t addDataSet(hid_t fileId, hid_t dataSpaceId);
	hid_t openDataSet(hid_t fileId);
	herr_t closeDataSet(hid_t dataSetId);
	herr_t readDataSet(hid_t dataSetId, double *data);
	herr_t writeDataSet(hid_t dataSetId, double *data);
	hid_t getDataSpace(hid_t dataSetId);
	hid_t addAttribute(hid_t dataSetId, hid_t dataSpaceId);
	herr_t closeAttribute(hid_t attributeId);

signals:

public slots:
};

#endif // AMHDF5DRIVER_H
