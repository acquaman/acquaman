#ifndef AMROI_H
#define AMROI_H

#include <QObject>
#include <QDebug>
#include "dataman/AMROIInfo.h"
#include "beamline/AMProcessVariable.h"

/*! This class is used to set PVs based on either an AMROIInfo class or through other means (ex: a detector view).  It is meant to encapsulate the different pieces of an ROI into a single class.  For instance, the energy, the low limit, the high limit, and the name are all useful to group together.

  \todo This class is not implementation independent.  Because there does not exist an AMControl that handles arbitrary QStrings, I have chosen to implement the entire thing using PVs. Once the AMControl class can handle all things required by this class I will port it over to AMControls and AMControlSets.
  */
class AMROI : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes a base name, number of elements, the region to be added and creates all the PVs.
	explicit AMROI(QString baseName, int elements, int number, QObject *parent = 0);

	/// Takes an AMROIInfo and sets the AMROI to match it.
	void fromInfo(const AMROIInfo &info);
	/// Takes the current state of the AMROI and returns an AMROIInfo.
	AMROIInfo toInfo();

	/// Returns the name of the region of interest.
	QString name() const { return pvNames_.first()->getString(); }
	/// Returns the current central energy of the region of interest.
	double energy() const { return energy_; }
	/// Returns the current scaling for the region of interst.
	double scale() const { return scale_; }
	/// Returns the lower bound of the region of interest as a channel number.
	int low() const { return pvLowerBounds_.first()->getInt(); }
	/// Returns the higher bound off the region of interest as a channel number.
	int high() const { return pvHigherBounds_.first()->getInt(); }
	/// Returns the current value of the ROI.
	double value() const { return value_; }

	/// Returns whether the entire region of interest has values.
	bool hasValues() const { return hasValues_; }

signals:
	/// Signal emitted with a status indicator of whether or not all the process variables have values in them.
	void roiHasValues();
	/// Notifier that the scale has changed.
	void scalerChanged(double);
	/// Signal emitting that the name PV has changed.
	void nameUpdate(QString);
	/// Signal emitting that the low PV has changed.
	void lowUpdate(int);
	/// Signal emitting that the high PV has changed.
	void highUpdate(int);
	/// Signal emitting the new value of the ROI.
	void valueUpdate(double);

public slots:
	/// Sets the name of the ROI and passes it to all PV's.
	void setName(QString name);
	/// Sets the energy of the ROI.
	void setEnergy(double energy) { energy_ = energy; }
	/// Sets the scaling factor for the region of interest.
	void setScale(double scale) { scale_ = scale; emit scalerChanged(scale); }
	/// Explicitly changes the low bound for the ROI and all the PV's.  Does not affect the energy or the width in any way.
	void setLow(int low);
	/// Overloaded function.  Sets the lower bound of the ROI based on a energy and uses the scaling factor to compute the channel number.  Sets the new value to all PV's and does not affect the energy or width in any way.
	void setLow(double low);
	/// Explicitly changes the high bound for the ROI and all the PV's.  Does not affect the energy or the width in any way.
	void setHigh(int high);
	/// Overloaded function.  Sets the upper bound of the ROI based on a energy and uses the scaling factor to compute the channel number.  Sets the new value to all PV's and does not affect the energy or width in any way.
	void setHigh(double high);
	/// Computes the low and high values for the region of interest based on the current energy and given width (expressed as a percentage).
	void computeLimits(double width);
	/// Sets the all the parameters for the region using the energy and width.
	void setRegion(QString name, double energy, double width);
	/// Overloaded function.  Sets all the parameters for the region with low and high bounds.
	void setRegion(QString name, int low, int high);
	/// Overloaded function.  Takes in an AMROIInfo and sets everything appropriately.
	void setRegion(const AMROIInfo &info);

protected slots:
	/// Sets the name if it is changed from the base.
	/// Used to compute the current value based on the current state of the PVs.
	void updateValue();
	/// Used to determine if all of the process variables have values in them or not.
	void onHasValuesChanged();

protected:
	/// Takes the names and creates all the PVs.
	void buildAllPVs(QString baseName, int elements, int number);

	/// The central energy of the region of interest.  Usually a known value of an emission line of an element.
	double energy_;
	/// THe scale that converts the energy and width of a region of interest into an upper and lower bounds.
	double scale_;
	/// The current integrated value of the region of interest based on the current energy and width.
	double value_;
	/// Holds the current state of whether the region of interest has values contained in it.
	bool hasValues_;

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
