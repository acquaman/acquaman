#ifndef VESPERSXASSCANCONFIGURATION_H
#define VESPERSXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

#include  <QMap>

class VESPERSXASScanConfiguration : public AMXASScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)
	Q_PROPERTY(int transmissionChoice READ transmissionChoice WRITE setTransmissionChoice)
	Q_PROPERTY(int incomingChoice READ incomingChoice WRITE setIncomingChoice)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XAS Scan Configuration")

public:

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement, FourElement };
	/// Enum for the ion chambers.  Used when choosing It and I0.
	enum IonChamber { Isplit = 0, Iprekb, Imini, Ipost };

	/// Constructor.
	Q_INVOKABLE explicit VESPERSXASScanConfiguration(QObject *parent=0);
	/// Copy constructor.
	VESPERSXASScanConfiguration(const VESPERSXASScanConfiguration &original);

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

	/// Returns the ion chamber name from its corresponding enum.
	QString ionChamberName(IonChamber chamber) { return ionChamberNames_.value(chamber); }

public slots:
	/// Adds a region to the XAS scan.  \param index is the region you are adding and \param start, \param delta, and \param end define the region.
	virtual bool addRegion(int index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end); }

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

protected:
	/// Fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice_;
	/// It ion chamber choice.
	IonChamber It_;
	/// I0 ion chamber choice.
	IonChamber I0_;

	/// Mapping between Ion chambers and their names.
	QMap<IonChamber, QString> ionChamberNames_;
};

#endif // VESPERSXASSCANCONFIGURATION_H
