#ifndef AMHDF5DATASET_H
#define AMHDF5DATASET_H

#include <QObject>

#include <hdf5.h>

#include <QString>
#include <QVector>

/// This class handles the data within an HDF5 file.  Each data set is unique and will have an ID based
/// on it's name.  Requires a valid file ID for any of it's functionality to be accessed.
class AMHDF5DataSet : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Requires a valid AMHDF5File id and name (path) in the file.
	explicit AMHDF5DataSet(hid_t fileId, const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5DataSet();

	/// Returns whether the data set is open.
	bool isOpen() const;
	/// Returns the id of the data set.
	hid_t id() const;
	/// Returns the data set name.
	QString name() const;

	/// Reads the data and puts it in the provided buffer.  The data buffer must have the necessary amount of space to read the data.
	bool readData(double *data) const;
	/// Writes data to the HDF5 file buffer.  Note that the data needs to be flushed or the data set closed before the data is written to disk.
	bool writeData(double *data);
	/// Fills the data with a specific value.
	bool fill(double value, double *data);

signals:

public slots:
	/// Creates a data set using its file ID and name.
	bool create(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum = QVector<hsize_t>());
	/// Opens a data set with it's provided file ID and name.
	bool open();
	/// Closes a data set.
	bool close();
	/// Flushes the contents of the data set to disk.
	bool flush();

protected:
	/// Method that returns the id of a copy of the data space that represents this data set.  It must be released after use.
	hid_t dataSpaceId() const;

	/// The file id this data set is attached to.
	hid_t fileId_;
	/// The name and path of the data set within the HDF5 file.
	QString name_;
	/// The id of this data set.
	hid_t id_;
};

#endif // AMHDF5DATASET_H
