#ifndef AMHDF5DATASPACEINFO_H
#define AMHDF5DATASPACEINFO_H

#include <QObject>

#include <hdf5.h>

#include <QVector>

#define AMHDF5DATASPACEINFO_INVALID_ID 670000
#define AMHDF5DATASPACEINFO_DOES_NOT_HAVE_VALID_TYPE 670001

/// Although you can't open a data space after creating and closing it, you may want to know the key
/// attributes of that space.  There are means of knowing that information through the API.
class AMHDF5DataSpaceInfo : public QObject
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
		Null = 2,
		Invalid = 4
	};

	/// Constructor.  Requires an id.  If an invalid id is provided then this will contain default values.
	AMHDF5DataSpaceInfo(hid_t spaceId, QObject *parent = 0);
	/// Destructor.
	virtual ~AMHDF5DataSpaceInfo();

	/// Returns the id of this data space info.
	hid_t id() const;
	/// Returns the type of this data space.
	Type type() const;
	/// Returns the rank of the data space.
	int rank() const;
	/// Returns the current dimensions.
	QVector<hsize_t> currentDimensions() const;
	/// Returns the maximum dimensions.
	QVector<hsize_t> maximumDimensions() const;

public slots:
	/// Refreshes the data.
	bool refresh();

protected:
	/// The id of the space.
	hid_t id_;
	/// The type of the data space.
	Type type_;
	/// The rank of the data space.
	int rank_;
	/// The current dimensions of the data space.
	QVector<hsize_t> currentDimensions_;
	/// The maximum dimensions of the data space.
	QVector<hsize_t> maximumDimensions_;
};

#endif // AMHDF5DATASPACEINFO_H
