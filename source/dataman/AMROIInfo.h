#ifndef AMROIINFO_H
#define AMROIINFO_H

#include "dataman/AMDbObject.h"

/// This class can be used to store the essential information of an AMROI that is used for spectral detectors (ex: Silicon Drift Detectors), without actually needing to have a live control object. This information includes the the name, centre energy and width of the region.
class AMROIInfo : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(QString name READ name WRITE setName)
	Q_PROPERTY(double energy READ energy WRITE setEnergy)
	Q_PROPERTY(double width READ width WRITE setWidth)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Saved ROI State")

public:
	/// Constructor.  Takes in the name, centre energy, and width of the region.
	AMROIInfo(const QString& name, double energy, double width, QObject *parent = 0);
	Q_INVOKABLE AMROIInfo(AMDatabase *db, int id);

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

#endif // AMROIINFO_H
