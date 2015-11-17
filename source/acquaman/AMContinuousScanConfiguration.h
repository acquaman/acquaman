#ifndef AMCONTINUOUSSCANCONFIGURATION_H
#define AMCONTINUOUSSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"

#include "util/AMOrderedList.h"
#include "dataman/AMScanAxis.h"

/// This is the generic scan configuration for all continuous scans.
class AMContinuousScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObjectList scanAxes READ dbReadScanAxes WRITE dbLoadScanAxes)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Base configuration for continuous axis scans.")

public:
	/// Constructor.  Builds a generic scan configuration that handles continuous motion based scans.
	AMContinuousScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	AMContinuousScanConfiguration(const AMContinuousScanConfiguration &original);
	/// Destructor.
	virtual ~AMContinuousScanConfiguration();

	/// Returns the scan axis at the given position.
	AMScanAxis *scanAxisAt(int index) const { return scanAxes_.at(index); }
	/// Returns the list of scan axes.
	QList<AMScanAxis *> scanAxes() const { return scanAxes_.toList(); }

signals:
	/// Notifier that an axis has been added to the scan configuration.
	void scanAxisAdded(AMScanAxis *);
	/// Notifier that an axis has been removed from the scan configuration.
	void scanAxisRemoved(AMScanAxis *);

public slots:
	/// Inserts a scan axis into the list.
	void insertScanAxis(int index, AMScanAxis *newAxis);
	/// Appends a scan axis to the end of the list.
	void appendScanAxis(AMScanAxis *newAxis);
	/// Overloaded.  Removes the scan axis provided.  Returns whether removal was successful.
	bool removeScanAxis(AMScanAxis *axis);
	/// Removes a scan axis from the list at the given index.  Returns the pointer to the axis.  Returns 0 if nothing is removed.
	AMScanAxis *removeScanAxis(int index);

protected:
	/// Returns a list of the AMDetectorInfo objects we store, for use by the database system in storeToDb() / loadFromDb().
	AMDbObjectList dbReadScanAxes();
	/// Called by the database system on loadFromDb() to give us our new set of AMDetectorlInfo objects.
	void dbLoadScanAxes(const AMDbObjectList &newScanAxes);

	/// The list of scan axes.
	AMOrderedList<AMScanAxis *> scanAxes_;
};

#endif // AMCONTINUOUSSCANCONFIGURATION_H
