#ifndef AMSCANAXISEXAFSREGION_H
#define AMSCANAXISEXAFSREGION_H

#include "dataman/AMScanAxisRegion.h"

class AMScanAxisEXAFSRegion : public AMScanAxisRegion
{
	Q_OBJECT

	Q_PROPERTY(bool inKSpace READ inKSpace WRITE setInKSpace)
	Q_PROPERTY(double edgeEnergy READ edgeEnergy WRITE setEdgeEnergy)
	Q_PROPERTY(double maximumTime READ maximumTime WRITE setMaximumTime)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Specific EXAFS region for EXAFS scan axes.")

public:
	/// Constructor.  Allows for maximum customization.  Extends the normal region by having a flag for knowing whether the region is in k-space or not.
	AMScanAxisEXAFSRegion(bool inKSpace = false, const AMNumber &edgeEnergy = AMNumber(AMNumber::Null), const AMNumber &start = AMNumber(AMNumber::Null), const AMNumber &step = AMNumber(AMNumber::Null), const AMNumber &end = AMNumber(AMNumber::Null), const AMNumber &time = AMNumber(AMNumber::Null), const AMNumber &maximumTime = AMNumber(AMNumber::Null), QObject *parent = 0);
	/// Constructor.  Creates a standard energy based region.
	AMScanAxisEXAFSRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, QObject *parent = 0);
	/// Copy constructor.
	AMScanAxisEXAFSRegion(const AMScanAxisEXAFSRegion &original);
	/// Destructor.
	virtual ~AMScanAxisEXAFSRegion(){}

	/// Returns whether this region is in k-space.
	bool inKSpace() const { return inKSpace_; }
	/// Returns the edge energy associated with this region.
	AMNumber edgeEnergy() const { return edgeEnergy_; }
	/// Returns the maximum time for the region.
	AMNumber maximumTime() const { return maximumTime_; }

signals:
	/// Notifier that the flag has changed.
	void inKSpaceChanged(bool);
	/// Notifier that the edge energy changed.
	void edgeEnergyChanged(const AMNumber &);
	/// Notifier that the maximum time changed.
	void maximumTimeChanged(const AMNumber &);

public slots:
	/// Sets the flag for whether this is a normal region or an extended region.
	void setInKSpace(bool flag);
	/// Sets the edge energy.
	void setEdgeEnergy(const AMNumber &energy);
	/// Sets the maximum time.
	void setMaximumTime(const AMNumber &newMaximumTime);

protected:

	/// Flag that holds whether this region is in k-space or not.
	bool inKSpace_;
	/// Flag holding the edge energy.
	AMNumber edgeEnergy_;
	/// Flag holding the maximum time.  Used for variable integration time calculations.
	AMNumber maximumTime_;

private:
	/// Switches the start, step, and end position based on what the current state of the inKSpace flag is set to.  Assumes all values are appropriate.
	void switchSpace();
	/// Takes a value in energy and returns the k-value.  Assumes edgeEnergy_ is valid.
	AMNumber toKSpace(const AMNumber &energy) const;
	/// Takes a value in k-space and returns the value in energy.  Assumes edgeEnergy_ is valid.
	AMNumber toEnergy(const AMNumber &k) const;
};

#endif // AMSCANAXISEXAFSREGION_H
