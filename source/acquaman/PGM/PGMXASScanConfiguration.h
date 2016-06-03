#ifndef PGMXASSCANCONFIGURATION_H
#define PGMXASSCANCONFIGURATION_H

#include "acquaman/AMGenericStepScanConfiguration.h"

/// The scan configuration for XAS on PGM.
class PGMXASScanConfiguration : public AMGenericStepScanConfiguration
{
	Q_OBJECT

public:
	/// Constructor.
	Q_INVOKABLE PGMXASScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	PGMXASScanConfiguration(const PGMXASScanConfiguration &original);
	/// Destructor.
	virtual ~PGMXASScanConfiguration(){}

	/// Returns a description of the scan technique.
	virtual QString technique() const { return "XAS"; }
	/// Returns a description of this scan configuration.
	virtual QString description() const { return "XAS Scan"; }
	/// Returns a detailed description of this scan configuration.
	virtual QString detailedDescription() const { return "VLSPGM XAS Scan"; }
	/// Returns whether this is an XAS technique scan
	virtual bool isXASScan() const { return true; }

	/// Returns a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration *createCopy() const;
	/// Returns a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController *createController();
	/// Returns a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

signals:

public slots:
};

#endif // PGMXASSCANCONFIGURATION_H
