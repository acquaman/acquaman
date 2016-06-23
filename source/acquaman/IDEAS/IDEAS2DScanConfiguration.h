#ifndef IDEAS2DSCANCONFIGURATION_H
#define IDEAS2DSCANCONFIGURATION_H

#include "acquaman/AMStepScanConfiguration.h"
#include "application/IDEAS/IDEAS.h"
#include "acquaman/IDEAS/IDEASScanConfiguration.h"

class IDEAS2DScanConfiguration : public AMStepScanConfiguration, public IDEASScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(AMDbObject* configurationDbObject READ dbReadScanConfigurationDbObject WRITE dbWriteScanConfigurationDbObject)
	Q_PROPERTY(QString header READ headerText WRITE setHeaderText)

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS 2D Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE IDEAS2DScanConfiguration(QObject *parent = 0);
	/// Copy constructor.
	IDEAS2DScanConfiguration(const IDEAS2DScanConfiguration &original);
	/// Detructor.
	virtual ~IDEAS2DScanConfiguration();

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns a pointer to a newly-created AMScanConfigurationView that is appropriate for viewing and editing this kind of scan configuration. Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanConfigurationView* createView();

	/// Returns the technique string.
	virtual QString technique() const;
	/// A human-readable description of this scan configuration. Can be re-implemented to provide more details. Used by scan action to set the title for the action view.
	virtual QString description() const;
	/// A human-readable synopsis of this scan configuration. Can be re-implemented to proved more details. Used by scan action to set the main text in the action view.
	virtual QString detailedDescription() const;

	/// Get a nice looking string that contains all the standard information in an XAS scan.   Used when exporting.
	QString headerText() const;

signals:
	/// Notifier that the total time estimate has changed.
	void totalTimeChanged(double);

protected slots:
	/// Computes the total time any time the regions list changes.
	void computeTotalTime() { computeTotalTimeImplementation(); }

protected:
	/// Computes the total estimated time for the scan.
	virtual void computeTotalTimeImplementation();
};

#endif // IDEAS2DSCANCONFIGURATION_H
