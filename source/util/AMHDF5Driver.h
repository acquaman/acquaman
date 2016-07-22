#ifndef AMHDF5DRIVER_H
#define AMHDF5DRIVER_H

#include <QObject>

#include "hdf5.h"

class AMHDF5Driver : public QObject
{
	Q_OBJECT

public:
	explicit AMHDF5Driver(QObject *parent = 0);

	hid_t openFile();
	herr_t closeFile(hid_t fileId);
signals:

public slots:
};

#endif // AMHDF5DRIVER_H
