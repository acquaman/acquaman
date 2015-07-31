#ifndef CLSJJSLITSCANCONFIGURATION_H
#define CLSJJSLITSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "beamline/CLS/CLSJJSlits.h"

class CLSJJSlitScanConfiguration : public AMGenericStepScanConfiguration
{
    Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE CLSJJSlitScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	CLSJJSlitScanConfiguration(const CLSJJSlitScanConfiguration &original);
	/// Destructor.
	virtual ~CLSJJSlitScanConfiguration();

	/// Returns the JJ slits being scanned.
	CLSJJSlits* jjSlits() const { return jjSlits_; }
	/// Returns the JJ slit blades of interest.
	CLSJJSlits::Control::Direction jjSlitBlades() const { return blades_; }
	/// Returns the JJ slit blades property of interest.
	CLSJJSlits::Control::Value jjSlitBladesProperty() const { return property_; }

	/// Returns a newly-created copy of this scan configuration.
	virtual AMScanConfiguration* createCopy() const;
	/// Returns a newly-created scan controller appropriate for executing this kind of scan configuration.
	virtual AMScanController* createController();
	/// Returns a newly-created view that is appropriate for viewing this kind of scan configuration.
	virtual AMScanConfigurationView* createView() const;

	/// Returns the technique.
	virtual QString technique() const { return "JJ Slit Scan"; }
	/// Returns the description.
	virtual QString description() const { return "JJ Slit Scan"; }
	/// Returns the detailed description.
	virtual QString detailedDescription() const { return "A step scan for a given JJ slit"; }

signals:
	/// Notifier that the JJ slits being scanned have changed.
	void jjSlitsChanged(CLSJJSlits *newSlits);

public slots:
	/// Sets the JJ slits to scan.
	void setJJSlits(CLSJJSlits *newSlits);
	/// Sets the JJ slit blades of interest.
	void setJJSlit

protected:
	/// The JJ slits being scanned.
	CLSJJSlits *jjSlits_;
	/// The blades of interest.
	CLSJJSlits::Control::Direction blades_;
	/// The blades property of interest.
	CLSJJSlits::Control::Value bladesProperty_;
};

#endif // CLSJJSLITSCANCONFIGURATION_H
