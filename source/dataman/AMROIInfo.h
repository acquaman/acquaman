#ifndef AMROIINFO_H
#define AMROIINFO_H

#include "dataman/AMDbObject.h"
#include "util/AMOrderedList.h"

/// This class can be used to store the essential information of an AMROI that is used for spectral detectors (ex: Silicon Drift Detectors), without actually needing to have a live control object. This information includes the the name, centre energy and width of the region.
class AMROIInfo : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(double width READ width WRITE setWidth)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Saved ROI State")

public:
	/// Constructor.  Takes in the name, centre energy, and width of the region.
	Q_INVOKABLE AMROIInfo(const QString& name, double energy, double width, QObject *parent = 0);

	/// Returns the centre energy of the region of interest.
	double energy() const { return energy_; }
	/// Returns the width of the region of interest.
	double width() const { return width_; }

public slots:
	/// Sets the centre energy of the region of interest.
	void setEnergy(double energy) { energy_ = energy; }
	/// Sets the width of the region of interst.
	void setWidth(double width) { width_ = width; }

protected:
	// Member variables.
	double energy_;
	double width_;
};

/// This is a container class for AMROIInfo, most commonly used to represent an arbitrary set of ROIs.
class AMROIInfoList : public AMDbObject, public AMOrderedList<AMROIInfo>
{
Q_OBJECT
	Q_PROPERTY(QString description READ description WRITE setDescription)
	Q_PROPERTY(AMDbObjectList roiInfos READ dbReadROIInfos WRITE dbLoadROIInfos)


public:
	/// Default constructor
	explicit AMROIInfoList(QObject *parent = 0);
	/// Loading-from-database constructor
	Q_INVOKABLE AMROIInfoList(AMDatabase* db, int id);
	/// Copy constructor
	AMROIInfoList(const AMROIInfoList& other);
	/// Assignment operator
	AMROIInfoList& operator=(const AMROIInfoList& other);

	/// Destructor
	~AMROIInfoList() {}

	/// A human-readable description for this set of ROIs
	QString description() const { return description_; }


	// Support for saving / restoring an AMROIInfoSet to the database
	////////////////////////////////

	/// Returns a list of pointers to the AMROIInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadROIInfos();
	/// Called by the database system on loadFromDb() to give us our new set of AMROIInfo objects. We copy these ones into our internal list and then delete them.
	void dbLoadROIInfos(const AMDbObjectList& newROIInfos);


signals:
	/// Forwarded from signalSource()->itemChanged(). Emitted when a ROI is replaced, OR after a ROI is accessed for modification and program execution returns back to the event loop.
	void roiValuesChanged(int index);
	/// Forwarded from signalSource()->itemAdded(). Emitted after a new ROI is added at \c index.
	void roiAdded(int index);
	/// Forwarded from signalSource()->itemRemoved(). Emitted after a ROI was removed (previously at \c index).
	void roiRemoved(int index);

public slots:
	/// Set the human-readable description
	void setDescription(const QString& description) {
		description_ = description;
		setModified(true);
	}


protected slots:
	/// Called when a ROI is accessed and potentially modified.
	void onROIValuesChanged(int index) {
		setModified(true);
		emit roiValuesChanged(index);
	}
	/// Called after a ROI is added at \c index
	void onROIAdded(int index) {
		setModified(true);
		emit roiAdded(index);
	}

	/// Called after a ROI is removed from \c index
	void onROIRemoved(int index) {
		setModified(true);
		emit roiRemoved(index);
	}

protected:
	QString description_;

};

#endif // AMROIINFO_H
