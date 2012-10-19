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


#ifndef VESPERSEXAFSSCANCONFIGURATION_H
#define VESPERSEXAFSSCANCONFIGURATION_H

#include "acquaman/AMEXAFSScanConfiguration.h"
#include "dataman/info/AMROIInfo.h"

/// This class is the VESPERS specific EXAFS scan configuration.
/*!
	This class extends the AMEXAFSScanConfiguration to configure any kind
	of XAS scan for the VESPERS beamline.  It is an energy scan that can
	be configured to run over just the edge of a given element (XANES) or it
	can be configured to run over the entire range of a given edge (EXAFS).
	Both types of scans have some subtle differences between how the scans are
	setup and these are accommodated.  You have the flexibility to choose which
	ion chamber you wish to use for I0 and It (for transmission mode) and also
	the choice of fluorescence detector that the scan will use if collecting
	the partial fluorescence yield.  It saves the edge, the energy, and a position
	that the scan will move to in the event there are different hot spots that
	you want to queue up.
  */

class VESPERSEXAFSScanConfiguration : public AMEXAFSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(double edgeEnergy READ energy WRITE setEnergy)
	Q_PROPERTY(bool goToPosition READ goToPosition WRITE setGoToPosition)
	Q_PROPERTY(double xPosition READ x WRITE setX)
	Q_PROPERTY(double yPosition READ y WRITE setY)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)
	Q_PROPERTY(QString rois READ readRoiList WRITE writeRoiList)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)
	Q_PROPERTY(bool useFixedTime READ useFixedTime WRITE setUseFixedTime)
	Q_PROPERTY(int numberOfScans READ numberOfScans WRITE setNumberOfScans)

	Q_CLASSINFO("useFixedTime", "upgradeDefault=false")
	Q_CLASSINFO("numberOfScans", "upgradeDefault=1")

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS EXAFS Scan Configuration")

public:
	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement = 1, FourElement = 2 };
	/// Enum for the ion chambers.  Used when choosing It and I0.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };

	/// Constructor.
	Q_INVOKABLE VESPERSEXAFSScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original);

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by AMBeamlineScanAction to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Returns whether we are going to export the spectra data sources or not.
	bool exportSpectraSources() const { return exportSpectraSources_; }

	/// Returns the current fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice() const { return fluorescenceDetectorChoice_; }
	/// Returns the current It ion chamber choice.
	IonChamber transmissionChoice() const { return It_; }
	/// Returns the current I0 ion chamber choice.
	IonChamber incomingChoice() const { return I0_; }
	/// Returns the name of the current edge.
	QString edge() const { return edge_; }
	/// Returns the edge energy for the scan.
	double energy() const { return energy_; }

	/// Returns the scan should move to a new position before starting the scan.
	bool goToPosition() const { return goToPosition_; }
	/// Returns the position that the scan should move to.
	QPair<double, double> position() const { return position_; }
	/// Returns the x coordinate of the scan position.
	double x() const { return position_.first; }
	/// Returns the y coordinate of the scan position.
	double y() const { return position_.second; }

	/// Returns whether the scan should use fixed or variable integration time.  The default is to use the variable integration time.
	bool useFixedTime() const { return useFixedTime_; }
	/// Returns the number of times this scan will be run.
	int numberOfScans() const { return numberOfScans_; }

	/// Returns the current total estimated time for a scan to complete.
	double totalTime() const { return totalTime_; }
	/// Returns the time offset.
	double timeOffset() const { return timeOffset_; }

	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return roiInfoList_; }

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }
	/// returns the AMControlInfo for the k-control.
	AMControlInfo kControlInfo() const { return exafsRegions()->defaultKControl()->toInfo(); }

	/// Get a pretty looking string of the current regions of interest.  Used primarily for exporting the list into the header of the file.
	QString readRoiList() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void writeRoiList(QString) {}

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void setHeaderText(QString) {}

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}

signals:
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChoiceChanged(FluorescenceDetector);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChoiceChanged(int);
	/// Notifier that the incoming choice has changed.
	void incomingChoiceChanged(IonChamber);
	/// Same signal.  Just passing as an int.
	void incomingChoiceChanged(int);
	/// Notifier that the transmition choice has changed.
	void transmissionChoiceChanged(IonChamber);
	/// Same signal.  Just passing as an int.
	void transmissionChoiceChanged(int);
	/// Notifier that the edge name has changed.
	void edgeChanged(QString);
	/// Notifier that the edge energy has changed.
	void energyChanged(double);
	/// Notifier about whether the scan will move somewhere specific before scanning.
	void gotoPositionChanged(bool);
	/// Notifier that the x coordinate of the position has changed.
	void xPositionChanged(double);
	/// Notifier that the y coordinate of the position has changed.
	void yPositionChanged(double);
	/// Notifier about whether the scan should use fixed or variabled integration time.
	void useFixedTimeChanged(bool);
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);
	/// Notifier that the number of scans has changed.
	void numberOfScansChanged(int);

public slots:

	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectorChoice(FluorescenceDetector detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectorChoice(int detector) { setFluorescenceDetectorChoice((FluorescenceDetector)detector); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(IonChamber It);
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((IonChamber)It); }
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(IonChamber I0);
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((IonChamber)I0); }
	/// Sets the current edge for the scan.
	void setEdge(QString edgeName);
	/// Sets the edge energy.
	void setEnergy(double edgeEnergy);

	/// Sets whether the scan should move to a new position before starting.
	void setGoToPosition(bool state);
	/// Sets the position the scan should move to before starting.
	void setPosition(QPair<double, double> pos);
	/// Overloaded.  Takes the x and y position explicitly.
	void setPosition(double xPos, double yPos) { setPosition(qMakePair(xPos, yPos)); }
	/// Sets the x coordinate of the starting position of the scan.
	void setX(double xPos);
	/// Sets the y coordinate of the starting position of the scan.
	void setY(double yPos);

	/// Sets whether the scan should use fixed or variable integration time for EXAFS.
	void setUseFixedTime(bool fixed);
	/// Sets the number of times this scan should be repeated.
	void setNumberOfScans(int num);

	/// Sets the time offset used for estimating the scan time.
	void setTimeOffset(double offset);

	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list);

	/// Sets whether we export the scan with the spectra included or not.
	void setExportSpectraSources(bool exportSpectra);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime();
	/// Makes sure that the variable integration time app is properly updated after a k-space region has been updated.
	void onEXAFSRegionsChanged();

protected:
	/// Fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice_;
	/// It ion chamber choice.
	IonChamber It_;
	/// I0 ion chamber choice.
	IonChamber I0_;

	/// The edge being scanned.
	QString edge_;
	/// The edge energy for the scan.
	double energy_;

	/// Holds whether the EXAFS scan will use fixed or variable integration time.
	bool useFixedTime_;

	/// Bool used to determine if the scan should go to a new location or stay wherever the current position is.
	bool goToPosition_;
	/// The position that the scan should go to when goToPosition_ is true.  \note Implementation detail: this currently assumes we are using the pseudomotor sample stage.
	QPair<double, double> position_;

	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;

	/// Holds the total time in seconds that the scan is estimated to take.
	double totalTime_;
	/// Holds the offset per point of extra time when doing a scan.
	double timeOffset_;

	/// Holds the number of times this scan should be repeated.
	int numberOfScans_;

	/// Flag holding whether we are exporting the spectra data sources or not.
	bool exportSpectraSources_;
};

#endif // VESPERSEXAFSSCANCONFIGURATION_H
