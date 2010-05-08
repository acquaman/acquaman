#ifndef ACQMAN_XASSCANCONFIGURATION_H
#define ACQMAN_XASSCANCONFIGURATION_H

#include <QObject>
#include "AMScanConfiguration.h"

/// An AMXASScanConfiguration is the parent class for any beamline that wants to implement a simple XAS (X-Ray Absorption Spectroscopy) scan.
/*!
  The AMXASScanConfiguration class is designed to allow beamline specific implementation classes to inherit it, but take advantage of a common method for configuring the energy regions.
  To this end, the class holds a QList of pointers to AMXASRegion.
  The AMXASRegion class inherits from AMRegion and optimizes the class to explicitly control beamline energy.
  Functions for adding and deleting regions, as well as editing regions are available.
  \todo Make a class on top of this (AMRegionScanConfiguration?) that uses just AMRegions? Then reimplement some of this...
  */
class AMXASScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT
public:
	/// Constructor, needs only a pointer to a QObject to act as a parent.
	AMXASScanConfiguration(QObject *parent = 0);
	/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double start(size_t index) const;
	/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
	double delta(size_t index) const;
	/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
	double end(size_t index) const;
	/// Returns a pointer to the region refered to by index. If an invalid index is given, returns NULL.
	AMXASRegion* region(size_t index) const;
	/// Returns a copy of the QList of regions.
	QList<AMXASRegion*> regions() { return regions_;}
	/// Returns a pointer to the QList of regions.
	QList<AMXASRegion*>* regionsPtr() { return &regions_;}

public slots:
	/// Sets the start value of the region refered to by index. Returns true if sucessful, returns false if the index is invalid or the energy is out of range.
	bool setStart(size_t index, double start);
	/// Sets the delta value of the region refered to by index. Returns true if sucessful, return false if the index is invalid or the delta is 0.
	bool setDelta(size_t index, double delta);
	/// Sets the end value of the region refered to by index. Returns true if succesful, returns false if the index is invalid or the energy is out of range.
	bool setEnd(size_t index, double end);
	/// Sets the region refered to by index. Returns true if successful, returns false if the index is invalid or the region pointer is NULL.
	bool setRegion(size_t index, AMXASRegion *region);
	/// Adds a region into position refered to by index and renumbers subsequent regions accordingly. Returns true if successful, returns false if the index is invalid.
	virtual bool addRegion(size_t index, AMXASRegion *region);
	/// Pure virtual function. Should be implemented in beamline specific subclasses as a convenience function for above.
	/// Creates a new region using start, delta, and end values then calls addRegion(index, *region).
	virtual bool addRegion(size_t index, double start, double delta, double end) = 0;
	/// Deletes the region refered to by index and renumbers subsequent regions accordingly. Returns true if successful, return false if index is invalid.
	bool deleteRegion(size_t index);

protected:
	/// Holds the list of AMXASRegion pointers.
	QList<AMXASRegion*> regions_;

//    QList<AMControlSet*> groups_;
};

#endif // ACQMAN_XASSCANCONFIGURATION_H
