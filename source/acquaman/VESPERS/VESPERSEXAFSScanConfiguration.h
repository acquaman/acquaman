#ifndef VESPERSEXAFSSCANCONFIGURATION_H
#define VESPERSEXAFSSCANCONFIGURATION_H

#include "acquaman/AMEXAFSScanConfiguration.h"
#include "dataman/info/AMROIInfo.h"

#include <QMap>

class VESPERSEXAFSScanConfiguration : public AMEXAFSScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(FluorescenceDetector fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)
	Q_PROPERTY(IonChamber transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(IonChamber incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(QString edge READ edge WRITE setEdge)
	Q_PROPERTY(double edgeEnergy READ energy WRITE setEnergy)
	Q_PROPERTY(bool goToPosition READ goToPosition WRITE setGoToPosition)
	Q_PROPERTY(double xPosition READ x WRITE setX)
	Q_PROPERTY(double yPosition READ y WRITE setY)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)
	Q_PROPERTY(QString rois READ readRoiList WRITE writeRoiList)

	Q_ENUMS(FluorescenceDetector)
	Q_ENUMS(IonChamber)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS EXAFS Scan Configuration")

public:
	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement, FourElement };
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

	/// Returns the ion chamber name from its corresponding enum.
	QString ionChamberName(IonChamber chamber) { return ionChamberNames_.value(chamber); }

	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return roiInfoList_; }

	/// Returns the AMControlInfo for the scanned region control.
	AMControlInfo regionControlInfo() const { return regions_->defaultControl()->toInfo(); }
	/// Returns the AMControlInfo for the time control.
	AMControlInfo timeControlInfo() const { return regions_->defaultTimeControl()->toInfo(); }

	/// Get a pretty looking string of the current regions of interest.  Used primarily for exporting the list into the header of the file.
	QString readRoiList() const;
	/// This function does nothing.  It is there to preserve the fact that the database needs to be able to read and write.
	void writeRoiList(QString) {}

	// Database loading and storing
	///////////////////////

	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}

public slots:

	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectorChoice(FluorescenceDetector detector) { fluorescenceDetectorChoice_ = detector; setModified(true); }
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetectorChoice(int detector) { setFluorescenceDetectorChoice((FluorescenceDetector)detector); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(IonChamber It) { It_ = It; setModified(true); }
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((IonChamber)It); }
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(IonChamber I0) { I0_ = I0; setModified(true); }
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((IonChamber)I0); }
	/// Sets the current edge for the scan.
	void setEdge(QString edgeName) { edge_ = edgeName; setModified(true); }
	/// Sets the edge energy.
	void setEnergy(double edgeEnergy) { energy_ = edgeEnergy; setModified(true); }

	/// Sets whether the scan should move to a new position before starting.
	void setGoToPosition(bool state) { goToPosition_ = state; setModified(true); }
	/// Sets the position the scan should move to before starting.
	void setPosition(QPair<double, double> pos) { position_ = pos; setModified(true); }
	/// Overloaded.  Takes the x and y position explicitly.
	void setPosition(double xPos, double yPos) { setPosition(qMakePair(xPos, yPos)); }
	/// Sets the x coordinate of the starting position of the scan.
	void setX(double xPos) { position_.first = xPos; setModified(true); }
	/// Sets the y coordinate of the starting position of the scan.
	void setY(double yPos) { position_.second = yPos; setModified(true); }

	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list) { roiInfoList_ = list; setModified(true); }

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

	/// Bool used to determine if the scan should go to a new location or stay wherever the current position is.
	bool goToPosition_;
	/// The position that the scan should go to when goToPosition_ is true.  \note Implementation detail: this currently assumes we are using the pseudomotor sample stage.
	QPair<double, double> position_;

	/// Mapping between Ion chambers and their names.
	QMap<IonChamber, QString> ionChamberNames_;

	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
};

#endif // VESPERSEXAFSSCANCONFIGURATION_H
