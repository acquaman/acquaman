#ifndef AMHDF5FILE_H
#define AMHDF5FILE_H

#include <QObject>

#include <hdf5.h>

#include <QString>

/// This is the base class that will relate all the other pieces of the HDF5 library.  Nothing can work
/// without having an open HDF5 file instance.
class AMHDF5File : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes the name (path) of the HDF5 file.  Note that this does not open the file.
	explicit AMHDF5File(const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5File();

	/// Returns whether the provided name points to an actual HDF5 file.
	bool isHDF5File() const;
signals:

public slots:

protected:
	/// The path and name of HDF5 file.
	QString name_;
	/// The file id.  This is heavily used by all the other parts of the API to create and associate objects in the HDF5 system.
	hid_t id_;
};

#endif // AMHDF5FILE_H
