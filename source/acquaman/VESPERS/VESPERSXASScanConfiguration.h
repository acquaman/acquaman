#ifndef VESPERSXASSCANCONFIGURATION_H
#define VESPERSXASSCANCONFIGURATION_H

#include "acquaman/AMXASScanConfiguration.h"

class VESPERSXASScanConfiguration : public AMXASScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetectorChoice READ fluorescenceDetectorChoice WRITE setFluorescenceDetectorChoice)

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS XAS Scan Configuration")

public:

	/// Enum for making the decision on what fluorescence detector the user wants to use.
	enum FluorescenceDetector { None = 0, SingleElement, FourElement };

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

public slots:
	/// Adds a region to the XAS scan.  \param index is the region you are adding and \param start, \param delta, and \param end define the region.
	virtual bool addRegion(int index, double start, double delta, double end) { return regions_->addRegion(index, start, delta, end);}

	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetectorChoice(FluorescenceDetector detector) { fluorescenceDetectorChoice_ = detector; }
	/// Overloaded.
	void setFluorescenceDetectorChoice(int detector) { setFluorescenceDetectorChoice((FluorescenceDetector)detector); }

protected:
	/// Fluorescence detector choice.
	FluorescenceDetector fluorescenceDetectorChoice_;
};

#endif // VESPERSXASSCANCONFIGURATION_H
