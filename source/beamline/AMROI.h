#ifndef AMROI_H
#define AMROI_H

#include <QObject>

#include "dataman/AMROIInfo.h"
#include "beamline/AMProcessVariable.h"

/// This class is used to set PVs based on either an AMROIInfo class or through other means (ex: a detector view).  It is meant to encapsulate the different pieces of an ROI into a single class.  For instance, the energy, the low limit, the high limit, and the name are all useful to group together.
class AMROI : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  All PV's provided to this class MUST be valid.
	explicit AMROI(QObject *parent = 0);

	/// Takes an AMROIInfo and sets the AMROI to match it.
	void fromInfo(const AMROIInfo &info);
	/// Takes the current state of the AMROI and returns an AMROIInfo.
	const AMROIInfo &toInfo();

	/// Returns the name of the region of interest.
	QString name() const { return name_; }
	/// Returns the current central energy of the region of interest.
	double energy() const { return energy_; }
	/// Returns the current width of the region of interest.
	double width() const { return width_; }
	/// Returns the current scaling for the region of interst.
	double scale() const { return scale_; }
	/// Returns the lower bound of the region of interest as a channel number.
	int low() const { return low_; }
	/// Returns the higher bound off the region of interest as a channel number.
	int high() const { return high_; }
	/// Returns the current value of the ROI.
	double value() const { return value_; }

	/// Sets the name PVs using the given QList of AMProcessVariables.
	void setNamePVs(QList<AMProcessVariable *> namePVs);
	/// Convenience function for single element detectors.
	void setNamePV(AMProcessVariable *namePV);
	/// Sets the lower bound PVs using the given QList of AMProcessVariables.
	void setLowerBoundPVs(QList<AMProcessVariable *> lowPVs);
	/// Convenience function for single element detectors.
	void setLowerBoundPV(AMProcessVariable *lowPV);
	/// Sets the higher bound PVs using the given QList of AMProcessVariables.
	void setHigherBoundPVs(QList<AMProcessVariable *> highPVs);
	/// Convenience function for single element detectors.
	void setHigherBoundPV(AMProcessVariable *highPV);
	/// Sets the value PVs using the given QList of AMProcessVariables.
	void setValuePVs(QList<AMProcessVariable *> valuePVs);
	/// Convenience function for single element detectors.
	void setValuePV(AMProcessVariable *valuePV);
	/// Sets all the PVs for the region of interest.
	void setAllPVs(QList<AMProcessVariable *> namePVs, QList<AMProcessVariable *> lowPVs, QList<AMProcessVariable *> highPVs, QList<AMProcessVariable *> valuePVs);
	/// Overloaded convenience function for single element detectors.
	void setAllPVs(AMProcessVariable *namePV, AMProcessVariable *lowPV, AMProcessVariable *highPV, AMProcessVariable *valuePV);

signals:

public slots:
	/// Sets the name of the ROI and passes it to all PV's.
	void setName(QString name);
	/// Sets the energy of the ROI.
	void setEnergy(double energy);
	/// Sets the width of the ROI.
	void setWidth(double width);
	/// Sets the scaling factor for the region of interest.
	void setScale(double scale) { scale_ = scale; }
	/// Explicitly changes the low bound for the ROI and all the PV's.  Does not affect the energy or the width in any way.
	void setLow(int low);
	/// Overloaded function.  Sets the lower bound of the ROI based on a energy and uses the scaling factor to compute the channel number.  Sets the new value to all PV's and does not affect the energy or width in any way.
	void setLow(double low);
	/// Explicitly changes the high bound for the ROI and all the PV's.  Does not affect the energy or the width in any way.
	void setHigh(int high);
	/// Overloaded function.  Sets the upper bound of the ROI based on a energy and uses the scaling factor to compute the channel number.  Sets the new value to all PV's and does not affect the energy or width in any way.
	void setHigh(double high);
	/// Computes the low and high values for the region of interest based on the current energy and width.
	void computeLimits();
	/// Sets the all the parameters for the region using the energy and width.
	void setRegion(QString name, double energy, double width);
	/// Overloaded function.  Sets all the parameters for the region with low and high bounds.
	void setRegion(QString name, int low, int high);

protected slots:
	/// Used to compute the current value based on the current state of the PVs.
	void updateValue();

protected:

	/// The name of the particular region of interest.
	QString name_;
	/// The central energy of the region of interest.  Usually a known value of an emission line of an element.
	double energy_;
	/// The width of the region of interest.  This value is expressed as a percentage.
	double width_;
	/// THe scale that converts the energy and width of a region of interest into an upper and lower bounds.
	double scale_;
	/// The actual channel number for the lower bound of the region of interest.
	int low_;
	/// The actual channel number for the higher bound of the region of interest.
	int high_;
	/// The current integrated value of the region of interest based on the current energy and width.
	double value_;

	/// A list of all the name PVs.  The size of the list will correspond to how many elements in the detector there is.
	QList<AMProcessVariable *> pvNames_;
	/// A list of all the lower bound PVs.  The size of the list will correspond to how many elements in the detector there is.
	QList<AMProcessVariable *> pvLowerBounds_;
	/// A list of all the higher bound PVs.  The size of the list will correspond to how many elements in the detector there is.
	QList<AMProcessVariable *> pvHigherBounds_;
	/// A list of all the current value PVs.  The size of the list will correspond to how many elements in the detector there is.
	QList<AMProcessVariable *> pvValues_;
};

#endif // AMROI_H
