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


#ifndef AM_XASSCANCONFIGURATION_H
#define AM_XASSCANCONFIGURATION_H

#include "acquaman/AMRegionScanConfiguration.h"

/// An AMXASScanConfiguration is the parent class for any beamline that wants to implement a simple XAS (X-Ray Absorption Spectroscopy) scan.
/*!
	This class is a very simple extension of the more general AMRegionScanConfiguration with some convenience functions
	and some XAS specific nomenclature.
  */
class AMXASScanConfiguration : public AMRegionScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic XAS Scan Configuration")

public:
	/// Constructor, needs a pointer to a QObject to act as a parent and needs to know whether it should new its own regions list.  This allows a way to not new the wrong type of regions list.
 	virtual ~AMXASScanConfiguration();
	AMXASScanConfiguration(QObject *parent = 0, bool setup = true);
	AMXASScanConfiguration(const AMXASScanConfiguration &original, bool setup = true);

	/// Convenience function for returning the beginning position of all regions.  Quick accessor for the start of the first region. If no regions are set, returns -1
	double startEnergy() const { return startValue(); }
	/// Convenience function for returning the ending position of all regions.  Quick accessor for the end of the final region. If no reginos are set, returns -1
	double endEnergy() const { return endValue(); }

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const {
		if(regionCount() != 0)
			return QString("XAS Scan from %1%3 to %2%4").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(regionUnits(0)).arg(regionUnits(regionCount()-1));
		else
			return QString("XAS Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const{
		return QString("XAS Scan from %1%3 to %2%4").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(regionUnits(0)).arg(regionUnits(regionCount()-1));
	}

	/// The auto-generated scan name. Can be re-implemented to customize for each scan type.
	virtual QString autoScanName() const{
		return QString("XAS");
	}

	/// Convenience function for regionControlInfo.  Returns the AMControlInfo for the energy control.
	virtual AMControlInfo energyControlInfo() const { return regionControlInfo(); }

	/// The scientific technique this configuration is for
	virtual QString technique() const{
		return "XAS";
	}

	/// Returns the regions list as an AMXASRegionsList.
	AMXASRegionsList* xasRegions() const { return (AMXASRegionsList *)regions_; }

public slots:
	/// Convenience function for setting the beginning position for all regions. Quick setter for the start of the first region. Returns false if there are no regions or the energy is out of range
	bool setStartEnergy(double startEnergy) { return setStartValue(startEnergy); }
	/// Convenience function for setting the ending position for all regions.  Quick setter for the end of the final region. Returns false if there are no regions or the energy is out of range
	bool setEndEnergy(double endEnergy) { return setEndValue(endEnergy); }
};

#endif // AM_XASSCANCONFIGURATION_H
