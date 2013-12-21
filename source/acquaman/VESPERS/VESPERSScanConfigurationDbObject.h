#ifndef VESPERSSCANCONFIGURATIONDBOBJECT_H
#define VESPERSSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/info/AMROIInfo.h"
#include "application/VESPERS/VESPERS.h"

/// This class is the common database object for all scan configurations for the VESPERS beamline.
/*!
	This class holds all of the persistent values that all scan configurations will need.  It is meant
	to consolidate all the duplications from all the scan configurations.  The only requirement for
	any class is that they'll need to set it in their Q_PROPERTY so that they properly load the
	configuration.
  */
class VESPERSScanConfigurationDbObject : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int motor READ motor WRITE setMotor)
	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE setFluorescenceDetector)
	Q_PROPERTY(int ccdDetector READ ccdDetector WRITE setCCDDetector)
	Q_PROPERTY(QString ccdFileName READ ccdFileName WRITE setCCDFileName)
	Q_PROPERTY(AMDbObject* roiInfoList READ dbGetROIInfoList WRITE dbLoadROIInfoList)
	Q_PROPERTY(double normalPosition READ normalPosition WRITE setNormalPosition)

	Q_CLASSINFO("normalPosition", "upgradeDefault=888888.88")

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS Scan Configuration Database Object")

public:
	/// Constructor.
 	virtual ~VESPERSScanConfigurationDbObject();
	Q_INVOKABLE VESPERSScanConfigurationDbObject(QObject *parent = 0);
	/// Copy Constructor.
	VESPERSScanConfigurationDbObject(const VESPERSScanConfigurationDbObject &original);

	/// Returns the current I0 ion chamber choice.
	VESPERS::IonChamber incomingChoice() const { return I0_; }
	/// Returns the current It ion chamber choice.
	VESPERS::IonChamber transmissionChoice() const { return It_; }
	/// Returns the current fluorescence detector choice.
	VESPERS::FluorescenceDetector fluorescenceDetector() const { return fluorescenceDetector_; }
	/// Returns the current motor choice.
	VESPERS::Motor motor() const { return motor_; }
	/// Returns what CCD the scan is using, if any.
	VESPERS::CCDDetector ccdDetector() const { return ccdDetector_; }
	/// Returns the CCD file name.
	QString ccdFileName() const { return ccdFileName_; }
	/// Returns the ROI list.  The list is empty if not using a fluorescence detector.
	AMROIInfoList roiList() const { return roiInfoList_; }
	/// The database reading member function.
	AMDbObject *dbGetROIInfoList() { return &roiInfoList_; }
	/// Don't need to do anything because dbGetROIList always returns a valid AMDbObject.
	void dbLoadROIInfoList(AMDbObject *) {}
	/// Returns the normal position.
	double normalPosition() const { return normalPosition_; }

signals:
	/// Notifier that the incoming choice has changed.
	void incomingChoiceChanged(VESPERS::IonChamber);
	/// Same signal.  Just passing as an int.
	void incomingChoiceChanged(int);
	/// Notifier that the transmition choice has changed.
	void transmissionChoiceChanged(VESPERS::IonChamber);
	/// Same signal.  Just passing as an int.
	void transmissionChoiceChanged(int);
	/// Notifier that the fluorescence choice has changed.
	void fluorescenceDetectorChanged(VESPERS::FluorescenceDetector);
	/// Same signal.  Just passing as an int.
	void fluorescenceDetectorChanged(int);
	/// Notifier that the motors choice has changed.
	void motorChanged(VESPERS::Motor);
	/// Same signal.  Just passing as an int.
	void motorChanged(int);
	/// Notifier that the flag for whether the CCD will be used has changed.
	void ccdDetectorChanged(VESPERS::CCDDetector);
	/// Same signal.  Just passing as an int.
	void ccdDetectorChanged(int);
	/// Notifier that the name of the CCD file name has changed.
	void ccdFileNameChanged(QString);
	/// Notifier that the normal position has changed.
	void normalPositionChanged(double);

public slots:
	/// Sets the choice for I0 ion chamber.
	void setIncomingChoice(VESPERS::IonChamber I0);
	/// Overloaded.  Used for database loading.
	void setIncomingChoice(int I0) { setIncomingChoice((VESPERS::IonChamber)I0); }
	/// Sets the choice for It ion chamber.
	void setTransmissionChoice(VESPERS::IonChamber It);
	/// Overloaded.  Used for database loading.
	void setTransmissionChoice(int It) { setTransmissionChoice((VESPERS::IonChamber)It); }
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(VESPERS::FluorescenceDetector detector);
	/// Overloaded.  Used for database loading.
	void setFluorescenceDetector(int detector) { setFluorescenceDetector((VESPERS::FluorescenceDetector)detector); }
	/// Sets the choice for the set of motors used for scanning.
	void setMotor(VESPERS::Motor choice);
	/// Overloaded.  Used for database loading.
	void setMotor(int choice) { setMotor((VESPERS::Motor)choice); }
	/// Sets whether the scan should be using the CCD or not.
	void setCCDDetector(VESPERS::CCDDetector ccd);
	/// Overloaded.  Used for database loading.
	void setCCDDetector(int ccd) { setCCDDetector((VESPERS::CCDDetector)ccd); }
	/// Sets the file name for the CCD files.
	void setCCDFileName(const QString &name);
	/// Sets the ROI list.
	void setRoiInfoList(const AMROIInfoList &list);
	/// Sets the normal position.
	void setNormalPosition(double newPosition);

protected:
	/// I0 ion chamber choice.
	VESPERS::IonChamber I0_;
	/// It ion chamber choice.
	VESPERS::IonChamber It_;
	/// Fluorescence detector choice.
	VESPERS::FluorescenceDetector fluorescenceDetector_;
	/// Motor choice for which set of motors will be used.
	VESPERS::Motor motor_;
	/// CCD choice whether the scan should use a CCD detector or not.
	VESPERS::CCDDetector ccdDetector_;
	/// The file name (minus number, path and extension of the file) for the CCD.
	QString ccdFileName_;
	/// The list holding all the current ROIs for the detector.
	AMROIInfoList roiInfoList_;
	/// The normal position of the scan.
	double normalPosition_;
};

#endif // VESPERSSCANCONFIGURATIONDBOBJECT_H
