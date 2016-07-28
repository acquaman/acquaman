#ifndef AMHDF5DATASPACE_H
#define AMHDF5DATASPACE_H

#include <QObject>

#include <hdf5.h>

#include <QVector>

#define AMHDF5DATASPACE_SPACE_ALREADY_OPEN 669000
#define AMHDF5DATASPACE_INVALID_SPACE_TYPE 669001
#define AMHDF5DATASPACE_SPACE_ALREADY_CLOSED 669002
#define AMHDF5DATASPACE_DIMENSIONS_DONT_MATCH 669003
#define AMHDF5DATASPACE_INCORRECT_DIMENSIONS 669004

/// This defines how a data space will be defined within an HDF5 file.  It isn't interacted with
/// as directly as many of the other parts but is important enough to compartmentalize as it's
/// own class.
class AMHDF5DataSpace : public QObject
{
	Q_OBJECT

public:
	/// Data space type.  This specifies how the data space should be defined.
	/*!
	 *	Scaler - single data element
	 *	Simple - normal array of data elements
	 *	Null - no data elements
	 */
	enum Type {
		Scaler = 0,
		Simple = 1,
		Null = 2
	};

	/// Constructor.
	explicit AMHDF5DataSpace(AMHDF5DataSpace::Type type, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5DataSpace();

	/// Returns the ID of the data space.
	hid_t id() const;
	/// Returns whether the data space is currently open.
	bool isOpen() const;

signals:

public slots:
	/// Creates a data space with the type provided in the constructor.
	bool create();
	/// Convenience method.  Creates a simple data space.  Must be of Type Simple otherwise this will return false.
	bool createSimple(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum = QVector<hsize_t>());
	/// Closes an open data space.
	bool close();
	/// Sets the dimensionality of a Simple data space.  Requires the rank, vector of the initial dimensions,
	/// and a vector of the maximum dimensions.  If the maximum dimensions is empty then it will be se to unlimited.
	bool setDimensionality(int rank, const QVector<hsize_t> &initial, const QVector<hsize_t> &maximum = QVector<hsize_t>());

protected:
	/// The type of this data space.
	Type type_;
	/// The id of the space.
	hid_t id_;
};

#endif // AMHDF5DATASPACE_H
