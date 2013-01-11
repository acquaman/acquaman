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


#ifndef VESPERSENDSTATIONCONFIGURATION_H
#define VESPERSENDSTATIONCONFIGURATION_H

#include <QObject>

#include <QPair>

#include "application/VESPERS/VESPERS.h"

/// This class handles the configuration of the beamline.  Based on it's state, various options for the beamline will become available.
/*!
	The basic concept is the user will choose a geometry for that will be used (single 45 degrees, double 45 degrees, etc.) and,
	based on the setup for the beamline remaining fairly static, offer possibilities for techniques and detectors.

	There are 5 geometries, 4 detectors, and 3 techniques that are offered and customizable.
  */
class VESPERSEndstationConfiguration : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit VESPERSEndstationConfiguration(QObject *parent = 0);

	/// Returns the current geometry.
	VESPERS::Geometry geometry() const { return current_; }

	// These getters return whether a particular piece of the endstation can be used or not.  These values change depending on the geometry.
	/// Returns whether the x and z sample stage can be used.
	bool canUseXAndZ() const { return xAndZ_.first; }
	/// Returns whether the h and v sample stage can be used.
	bool canUseHAndV() const { return hAndV_.first; }
	/// Returns whether the big beam sample stage can be used.
	bool canUseBigBeam() const { return bigBeam_.first; }
	/// Returns whether XAS can be performed.
	bool canUseXAS() const { return xas_.first; }
	/// Returns whether XRF can be performed.
	bool canUseXRF() const { return xrf_.first; }
	/// Returns whether XRD can be performed.
	bool canUseXRD() const { return xrd_.first; }
	/// Returns whether the ion chambers can be used.
	bool canUseIonChambers() const { return ionChambers_.first; }
	/// Returns whether the single element vortex detector can be used.
	bool canUseSingleElementVortex() const { return vortex1E_.first; }
	/// Returns whether the four element vortex detector can be used.
	bool canUseFourElementVortex() const { return vortex4E_.first; }
	/// Returns whether the Roper CCD can be used.
	bool canUseRoperCCD() const { return roperCCD_.first; }

	// These getters return whether the particular piece is actually in use.  Returns false if canUse() is false.
	/// Returns whether the x and z sample stage is in use.  Returns false if canUse is false.
	bool usingXAndZ() const { return xAndZ_.first && xAndZ_.second; }
	/// Returns whether the h and v sample stage is in use.  Returns false if canUse is false.
	bool usingHAndV() const { return hAndV_.first && hAndV_.second; }
	/// Return whether the big beam sample stage is in use.  Returns false if canUse is false.
	bool usingBigBeam() const { return bigBeam_.first && bigBeam_.second; }
	/// Returns whether XAS is the technique being performed.  Returns false if canUse is false.
	bool usingXAS() const { return xas_.first && xas_.second; }
	/// Returns whether XRF is the technique being performed.  Returns false if canUse is false.
	bool usingXRF() const { return xrf_.first && xrf_.second; }
	/// Returns whether XRD is the technique being performed.  Returns false if canUse is false.
	bool usingXRD() const { return xrd_.first && xrd_.second; }
	/// Returns whether the ion chambers are in use.  Returns false if canUse is false.
	bool usingIonChambers() const { return ionChambers_.first && ionChambers_.second; }
	/// Returns whether the single element vortex is in use.  Returns false if canUse is false.
	bool usingSingleElementVortex() const { return vortex1E_.first && vortex1E_.second; }
	/// Returns whether the four element vortex is in use.  Returns false if canUse is false.
	bool usingFourElementVortex() const { return vortex4E_.first && vortex4E_.second; }
	/// Returns whether the CCD is in use.  Returns false if canUse is false.
	bool usingRoperCCD() const { return roperCCD_.first && roperCCD_.second; }

signals:
	/// Notifier that the geometry changed when using setGeometry.  The selections on detector choice and technique choice are reset.
	void geometryChanged();
	/// Notifier that the configuration has changed.
	void configurationChanged();

	/// Notifier for whether the x and z sample stage is now in use.
	void usingXAndZChanged(bool);
	/// Notifier for whether the h and v sample stage is now in use.
	void usingHAndVChanged(bool);
	/// Notifier for whether the big beam sample stage is now in use.
	void usingBigBeamChanged(bool);
	/// Notifier about whether XAS will be used now.
	void usingXASChanged(bool);
	/// Notifier about whether XRF will be used now.
	void usingXRFChanged(bool);
	/// Notifier about whether XRD will be used now.
	void usingXRDChanged(bool);
	/// Notifier about whether the ion chambers are now in use.
	void usingIonChambersChanged(bool);
	/// Notifier about whether the single element vortex is now in use.
	void usingSingleElementVortexChanged(bool);
	/// Notifier about whether the four element vortex is now in use.
	void usingFourElementVortexChanged(bool);
	/// Notifier about whether the Roper CCD is now in use.
	void usingRoperCCDChanged(bool);

public slots:
	/// Sets the geometry.  Assigns the appropriate values on whether a piece can be used or not.
	void setGeometry(VESPERS::Geometry newGeometry);

	/// Sets whether the x and z sample stage will be used.  Sets it to false automatically if canUse is false.
	void setUsingXAndZ(bool use);
	/// Sets whether the h and v sample stage will be used.  Sets it to false automatically if canUse is false.
	void setUsingHAndV(bool use);
	/// Sets whether the big beam sample stage will be used.  Sets it to false automatically if canUse is false.
	void setUsingBigBeam(bool use);
	/// Sets whether the XAS technique will be used.  Sets it to false automatically if canUse is false.
	void setUsingXAS(bool use);
	/// Sets whether the XRF technique will be used.  Sets it to false automatically if canUse is false.
	void setUsingXRF(bool use);
	/// Sets whether the XRD technique will be used.  Sets it to false automatically if canUse is false.
	void setUsingXRD(bool use);
	/// Sets whether the ion chambers will be used.  Sets it to false automatically if canUse is false.
	void setUsingIonChambers(bool use);
	/// Sets whether the single element vortex detector will be used.  Sets it to false automatically if canUse is false.
	void setUsingSingleElementVortex(bool use);
	/// Sets whether the four element vortex detector will be used.  Sets it to false automatically if canUse is false.
	void setUsingFourElementVortex(bool use);
	/// Sets whether the Roper CCD will be used.  Sets it to false automatically if canUse is false.
	void setUsingRoperCCD(bool use);

protected:
	/// This holds what the current desired geometry is.
	VESPERS::Geometry current_;

	/// The following are pairs of booleans where the first one is whether or not the value can be changed, and the second is whether or not the value is valid or not.
	/// Pair that handles whether the X and Z sample stage can be used.
	QPair<bool, bool> xAndZ_;
	/// Pair that handles whether the H and V sample stage can be used.
	QPair<bool, bool> hAndV_;
	/// Pair that handles whether the big beam sample stage can be used.
	QPair<bool, bool> bigBeam_;
	/// Pair that handles whether XAS can be used.
	QPair<bool, bool> xas_;
	/// Pair that handles whether XRF mapping can be done.
	QPair<bool, bool> xrf_;
	/// Pair that handles whether XRD can be done.
	QPair<bool, bool> xrd_;
	/// Pair that handles whether the ion chambers can be used.
	QPair<bool, bool> ionChambers_;
	/// Pair that handles whether the single element vortex can be used.
	QPair<bool, bool> vortex1E_;
	/// Pair that handles whether the four element vortex can be used.
	QPair<bool, bool> vortex4E_;
	/// Pair that handles whether the Roper CCD can be used.
	QPair<bool, bool> roperCCD_;
};

#endif // VESPERSENDSTATIONCONFIGURATION_H
