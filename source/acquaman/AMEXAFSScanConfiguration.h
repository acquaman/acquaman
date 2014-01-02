/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMEXAFSSCANCONFIGURATION_H
#define AMEXAFSSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

/// An AMEXAFSScanConfiguration is the parent class for any beamline that wants to implement a general XAS (Xray Absorption Spectroscopy) scans.
/*!
	The important distinction between AMEXAFSScanConfiguration and AMXASScanConfiguration is that allows you to perform
	a scan into the extended region using the nomenclature that scientists use to setup scans of that type.  Although
	it is still an energy scan, the space is scanned is through the reciprocal space (AKA K-space) and hence regions are
	generally setup to go from 1k to 10k in steps of 0.01k.  This distinction is the main reason why it needs its own scan
	configuration.  Rather then moving uniformly through enegy space, it moves uniformly through reciprocal space.  The other
	distinction is that EXAFS scans are usually combination scans of both edge and extended region scans (XAS+EXAFS, or in
	hard xray language XANES+EXAFS) which means that the regions will likely have something that goes over the edge in energy
	space and then move into the extended region in K-space.

	This class extends of the AMXASScanConfiguration to accomodate the scan as described above.
  */
class AMEXAFSScanConfiguration : public AMXASScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic EXAFS Scan Configuration")

public:
	/// Constructor, needs a pointer to a QObject to act as a parent and needs to know whether it should new its own regions list.  This allows a way to not new the wrong type of regions list.
 	virtual ~AMEXAFSScanConfiguration();
	AMEXAFSScanConfiguration(QObject *parent = 0, bool setup = true);
	AMEXAFSScanConfiguration(const AMEXAFSScanConfiguration &original, bool setup = true);

	/// Returns the type of the region referred to by \param index.
	AMEXAFSRegion::RegionType regionType(int index) const { return exafsRegions()->type(index); }
	/// Returns the edge energy for the region referred to by \param index.
	double regionEdgeEnergy(int index) const { return exafsRegions()->edgeEnergy(index); }
	/// Returns whether the region referred to by \param index uses an absolute or relative energy.
	bool regionIsRelative(int index) const { return exafsRegions()->isRelative(index); }
	/// Returns the energy units of the region referred to by \param index.  This is necessary if you want to know the units of energy space even if the region itself is in k-space.
	QString regionEnergyUnits(int index) const { return exafsRegions()->energyUnits(index); }
	/// Returns the region start point, referred to by \param index, in the space indicated by \param type.
	double regionStartByType(int index, AMEXAFSRegion::RegionType type) const { return exafsRegions()->startByType(index, type); }
	/// Returns the region end point, referred to by \param index, in the space indicated by \param type.
	double regionEndByType(int index, AMEXAFSRegion::RegionType type) const { return exafsRegions()->endByType(index, type); }

	/// Re-implemented for EXAFS.  Quick accessor for the start of the first region. Will always return an energy.  If no regions are set, returns -1
	virtual double startValue() const;
	/// Re-implemented for EXAFS.  Quick accessor for the end of the final region. Will always return an energy.  If no reginos are set, returns -1
	virtual double endValue() const;

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		return QString("%5 Scan from %1%3 to %2%4")
				.arg(regionStart(0))
				.arg(regionEnd(regionCount()-1))
				.arg(regionUnits(0))
				.arg(regionUnits(regionCount()-1))
				.arg(exafsRegions()->hasKSpace() ? "EXAFS" : "XANES");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("%5 Scan from %1%3 to %2%4")
				.arg(regionStart(0))
				.arg(regionEnd(regionCount()-1))
				.arg(regionUnits(0))
				.arg(regionUnits(regionCount()-1))
				.arg(exafsRegions()->hasKSpace() ? "EXAFS" : "XANES");
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString(exafsRegions()->hasKSpace() ? "EXAFS" : "XANES");
	}

	/// Pure virtual function requiring the k-control info to be returned.
	virtual AMControlInfo kControlInfo() const = 0;

	/// Convenience function for regionControlInfo.  Returns the AMControlInfo for the energy control.
	virtual AMControlInfo energyControlInfo() const { return regionControlInfo(); }

	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return exafsRegions()->hasKSpace() ? "EXAFS" : "XANES";
	}

	/// Returns the regions list as an AMEXAFSRegionsList.
	AMEXAFSRegionsList *exafsRegions() const { return (AMEXAFSRegionsList *)regions_; }

public slots:
	/// Sets the type of the region referred to by \param index.
	bool setRegionType(int index, AMEXAFSRegion::RegionType type) { return exafsRegions()->setType(index, type); }
	/// Sets the edge energy of the region referred to by \param index.
	bool setRegionEdgeEnergy(int index, double energy) { return exafsRegions()->setEdgeEnergy(index, energy); }
	/// Sets whether the region uses absolute or relative energy.
	bool setRegionIsRelative(int index, bool isRelative) { return exafsRegions()->setRelative(index, isRelative); }
	/// Sets the start point of the region referred to by \param index based on the space indicated by \param type.
	bool setRegionStartByType(int index, double start, AMEXAFSRegion::RegionType type) { return exafsRegions()->setStartByType(index, start, type); }
	/// Sets the end point of the region referred to by \param index based on the space indicated by \param type.
	bool setRegionEndByType(int index, double end, AMEXAFSRegion::RegionType type) { return exafsRegions()->setEndByType(index, end, type); }

	/// Quick setter for the start of the first region. Returns false if there are no regions or the energy is out of range
	virtual bool setStartValue(double startValue);
	/// Quick setter for the end of the final region. Returns false if there are no regions or the energy is out of range
	virtual bool setEndValue(double endValue);

protected:
	/*!
		These are virtual functions in case a specific format or different information associated with the region is required.  The current
		way that they are stored is as a comma separated string with an ID tag as the first element and then the following pieces of the region
		in the following order:

		   1) Type (either Energy space or kSpace)
		   2) Edge energy (in the units given by region units)
		   3) Whether the energy is relative or absolute (true = relative, false = absolute).
		   4) Region starting point in energy space.
		   5) Region delta between points.
		   6) Region ending point in energy space.
		   7) Whether the region uses elastic starting points.
		   8) Whether the region uses elastic ending points.
		   9) The time spent on each point in the region.
		   10) The units for the scanned element of the region.  This is the units of energy space even if the region is itself in kSpace.
		   11) The units of time spent on each point.
	  */
	/// This returns a string that describes the current regions. Regions are stored in order as common separated start, delta, and end values. New lines divide regions. Empty string is no regions.
	virtual QString dbReadRegions() const;
	/// When loadFromDb() is called, this receives the string describing the regions and restores them.
	virtual void dbLoadRegions(const QString &exafsRegionsString);
};

#endif // AMEXAFSSCANCONFIGURATION_H
