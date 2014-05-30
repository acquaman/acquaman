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


#ifndef REIXSXASSCANCONFIGURATION_H
#define REIXSXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

/// Scan configuration for XAS scans on REIXS
class REIXSXASScanConfiguration : public AMXASScanConfiguration
{
    Q_OBJECT

	// The following properties should be moved upwards if we can figure out how to work on this together, and determine what should be stored at the AMScanConfiguration level.
	Q_PROPERTY(QString userScanName READ userScanName WRITE setUserScanName)
	Q_PROPERTY(int scanNumber READ scanNumber WRITE setScanNumber)
	Q_PROPERTY(int sampleId READ sampleId WRITE setSampleId)
	Q_PROPERTY(bool namedAutomatically READ namedAutomatically WRITE setNamedAutomatically)

	Q_PROPERTY(double slitWidth READ slitWidth WRITE setSlitWidth)
	Q_PROPERTY(bool applySlitWidth READ applySlitWidth WRITE setApplySlitWidth)
	Q_PROPERTY(int monoGrating READ monoGrating WRITE setMonoGrating)
	Q_PROPERTY(bool applyMonoGrating READ applyMonoGrating WRITE setApplyMonoGrating)
	Q_PROPERTY(int monoMirror READ monoMirror WRITE setMonoMirror)
	Q_PROPERTY(bool applyMonoMirror READ applyMonoMirror WRITE setApplyMonoMirror)
	Q_PROPERTY(int polarization READ polarization WRITE setPolarization)
	Q_PROPERTY(double polarizationAngle READ polarizationAngle WRITE setPolarizationAngle)
	Q_PROPERTY(bool applyPolarization READ applyPolarization WRITE setApplyPolarization)

	Q_CLASSINFO("AMDbObject_Attributes", "description=REIXS XAS Scan Configuration")

public:
	Q_INVOKABLE explicit REIXSXASScanConfiguration(QObject *parent = 0);
	virtual ~REIXSXASScanConfiguration();

	/// Copy constructor
	REIXSXASScanConfiguration(const REIXSXASScanConfiguration& other);
	/// Creates a fresh copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;

	/// Creates a scan controller capable of running this configuration
	virtual AMScanController* createController();
	/// Creates a view to edit the scan configuration
	virtual AMScanConfigurationView* createView();


	// Scan configuration properties
	/////////////////////////

	/// Meta-data information to pre-set in the scan.
	int scanNumber() const { return scanNumber_; }
	/// Meta-data information to pre-set in the scan.
	int sampleId() const { return sampleId_; }
	/// True if we should generate the scan name, number, and sampleId automatically (as best we can; we'll do this based on the last sample move)
	bool namedAutomatically() const { return namedAutomatically_; }


	double slitWidth() const { return slitWidth_; }
	bool applySlitWidth() const { return applySlitWidth_; }
	int monoGrating() const { return monoGrating_; }
	bool applyMonoGrating() const { return applyMonoGrating_; }
	int monoMirror() const { return monoMirror_; }
	bool applyMonoMirror() const { return applyMonoMirror_; }
	int polarization() const { return polarization_; }
	double polarizationAngle() const { return polarizationAngle_; }
	bool applyPolarization() const { return applyPolarization_; }


	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo regionControlInfo() const { return AMControlInfo(); }
	/// Unused, but required by AMRegionScanConfiguration
	virtual AMControlInfo timeControlInfo() const { return AMControlInfo(); }

signals:

public slots:

	void setScanNumber(int number) { if(number == scanNumber_) return; scanNumber_ = number; setModified(true); emit configurationChanged(); }
	void setSampleId(int sampleId) { if(sampleId == sampleId_) return; sampleId_ = sampleId; setModified(true); emit configurationChanged(); }
	void setNamedAutomatically(bool autoOn) { if(autoOn == namedAutomatically_) return;	namedAutomatically_ = autoOn; setModified(true); emit configurationChanged(); }

	void setSlitWidth(double slitWidth) { if(slitWidth == slitWidth_) return; slitWidth_ = slitWidth; setModified(true); emit configurationChanged(); }
	void setApplySlitWidth(bool applySlitWidth) { if(applySlitWidth == applySlitWidth_) return; applySlitWidth_ = applySlitWidth; setModified(true); emit configurationChanged(); }
	void setMonoGrating(int monoGrating) { if(monoGrating == monoGrating_) return; monoGrating_ = monoGrating; setModified(true); emit configurationChanged(); }
	void setApplyMonoGrating(bool applyMonoGrating) { if(applyMonoGrating == applyMonoGrating_) return; applyMonoGrating_ = applyMonoGrating; setModified(true); emit configurationChanged(); }
	void setMonoMirror(int monoMirror) { if(monoMirror == monoMirror_) return; monoMirror_ = monoMirror; setModified(true); emit configurationChanged(); }
	void setApplyMonoMirror(bool applyMonoMirror) { if(applyMonoMirror == applyMonoMirror_) return; applyMonoMirror_ = applyMonoMirror; setModified(true); emit configurationChanged(); }
	void setPolarization(int polarization) { if(polarization == polarization_) return; polarization_ = polarization; setModified(true); emit configurationChanged(); }
	void setPolarizationAngle(double polarizationAngle) { if(polarizationAngle == polarizationAngle_) return; polarizationAngle_ = polarizationAngle; setModified(true); emit configurationChanged(); }
	void setApplyPolarization(bool applyPolarization) { if(applyPolarization == applyPolarization_) return; applyPolarization_ = applyPolarization; setModified(true); emit configurationChanged(); }

protected:

	int scanNumber_;
	int sampleId_;
	bool namedAutomatically_;

	double slitWidth_;
	bool applySlitWidth_;
	int monoGrating_;
	bool applyMonoGrating_;
	int monoMirror_;
	bool applyMonoMirror_;
	int polarization_;
	double polarizationAngle_;
	bool applyPolarization_;


};

#endif // REIXSXASSCANCONFIGURATION_H
