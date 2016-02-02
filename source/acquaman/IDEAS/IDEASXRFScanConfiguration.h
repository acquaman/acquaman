/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef IDEASXRFSCANCONFIGURATION_H
#define IDEASXRFSCANCONFIGURATION_H

#include "acquaman/AMScanConfiguration.h"
#include "dataman/info/AMControlInfoList.h"
#include "application/IDEAS/IDEAS.h"

class IDEASXRFScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
	Q_PROPERTY(AMDbObject* xrfDetectorInfo READ dbReadXRFDetectorInfo WRITE dbLoadXRFDetectorInfo)

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS XRF Scan Configuration")

public:
	/// The constructor used for database loading.
	Q_INVOKABLE explicit IDEASXRFScanConfiguration(QObject *parent = 0);
	/// Default constructor.
	IDEASXRFScanConfiguration(AMDetectorInfo detectorInfo, QObject *parent = 0);
	/// Copy constructor.
	IDEASXRFScanConfiguration(const IDEASXRFScanConfiguration &original);
	/// Destructor.
	virtual ~IDEASXRFScanConfiguration();

	/// Returns the detector info for the current detector.
	AMDetectorInfo detectorInfo() const { return xrfDetectorInfo_; }

	/// Returns a new instance of the scan configuration.
	virtual AMScanConfiguration *createCopy() const;

	/// Returns a new instance of the scan controller.
	virtual AMScanController *createController();

	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details.
	virtual QString description() const {
			return QString("XRF Free Run Scan");
	}

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns the integration time.
	double integrationTime() const { return integrationTime_; }
	/// Returns the maximum energy in eV.
	double maximumEnergy() const { return maxEnergy_; }
	/// Returns the peaking time.
	double peakingTime() const { return peakingTime_; }

	/// Returns the scan notes
	QString scanNotes() const { return scanNotes_; }
	/// Returns the scan name
	QString scanName() const { return scanName_; }
	/// Returns the scan number
	int scanNumber() const { return scanNumber_; }
	/// Returns initial positions
	AMControlInfoList positions() const { return positions_; }

	/// Returns the current fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }

signals:
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChanged(IDEAS::FluorescenceDetectors);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChanged(int);

public slots:
	/// Sets the detector info to the given detector info.
	void setDetectorInfo(AMDetectorInfo info) { xrfDetectorInfo_ = info; }
	/// Sets the integration time.
	void setIntegrationTime(double time) { integrationTime_ = time; }
	/// Sets the maximum energy.
	void setMaximumEnergy(double energy) { maxEnergy_ = energy; }
	/// Sets the peaking time.
	void setPeakingTime(double time) { peakingTime_ = time; }

	/// Sets the scan notes
	void setScanNotes(QString notes)  {  scanNotes_ = notes; }
	/// Sets the scan name
	void setScanName(QString name)  {  scanName_ = name; }
	/// Sets the scan number
	void setScanNumber(int number)  {  scanNumber_ = number;}

	/// sets initial positions
	void setPositions(AMControlInfoList positions) { positions_ = positions; }

	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(IDEAS::FluorescenceDetectors detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((IDEAS::FluorescenceDetectors)detector); }


protected:
	/// Returns an AMDbObject pointer to the detector info.
	AMDbObject *dbReadXRFDetectorInfo() { return &xrfDetectorInfo_; }
	/// Empty function since it will never be called.
	void dbLoadXRFDetectorInfo(AMDbObject *) {}

	// Member variables.
	/// Detector info member variable.
	AMDetectorInfo xrfDetectorInfo_;

	/// The integration time.
	double integrationTime_;
	/// The maximum energy.  Stored in eV.
	double maxEnergy_;
	/// The peaking time.
	double peakingTime_;

	/// The scan notes
	QString scanNotes_;
	/// The scan name
	QString scanName_;
	/// the scan number
	int scanNumber_;
	/// Beamline conditions at the time a scan is started
	AMControlInfoList positions_;
	/// Fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector_;
};

//Q_DECLARE_OPERATORS_FOR_FLAGS(IDEASXASScanConfiguration::FluorescenceDetectors)

#endif // IDEASXRFSCANCONFIGURATION_H
