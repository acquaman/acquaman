#ifndef SXRMB2DMAPSCANCONFIGURATION_H
#define SXRMB2DMAPSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "acquaman/SXRMB/SXRMBScanConfiguration.h"

/// This class holds the configuration of a standard 2D map.
class SXRMB2DMapScanConfiguration : public AMStepScanConfiguration, public SXRMBScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB 2D Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE SXRMB2DMapScanConfiguration(QObject *parent = 0);
	/// Copy Constuctor.
	SXRMB2DMapScanConfiguration(const SXRMB2DMapScanConfiguration &original);
	/// Destructor.
	virtual ~SXRMB2DMapScanConfiguration() {}

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	virtual QString headerText() const;

	/// Returns whether we are using Ascii or SMAK data formats for auto-exporting.
	bool exportAsAscii() const { return exportAsAscii_; }

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

public slots:
	/// Sets which data file format we use for auto-export.  True = Ascii, false = SMAK.
	void setExportAsAscii(bool exportAsAscii);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }

protected:
	/// Method that does all the calculations for calculating the estimated scan time.
	virtual void computeTotalTimeImplementation();

	/// Flag holding whether we are exporting as Ascii or SMAK.
	bool exportAsAscii_;
};

#endif // SXRMB2DMAPSCANCONFIGURATION_H
