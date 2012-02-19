#ifndef VESPERS2DSCANCONFIGURATION_H
#define VESPERS2DSCANCONFIGURATION_H

#include "acquaman/AM2DScanConfiguration.h"

class VESPERS2DScanConfiguration : public AM2DScanConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=VESPERS 2D Scan Configuration")

public:
	/// Constructor.
	Q_INVOKABLE VESPERS2DScanConfiguration(QObject *parent = 0);
	/// Copy Constructor.
	VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original);

	/// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
	virtual AMScanConfiguration* createCopy() const;

	/// Returns a pointer to a newly-created AMScanController that is appropriate for executing this kind of scan configuration.  The controller should be initialized to use this scan configuration object as its scan configuration.  Ownership of the new controller becomes the responsibility of the caller.
	virtual AMScanController* createController();

	/// Returns the x-axis name.
	virtual QString xAxisName() const { return ""; }
	/// Returns the x-axis units.
	virtual QString xAxisUnits() const { return ""; }
	/// Returns the y-axis name.
	virtual QString yAxisName() const  { return ""; }
	/// Returns teh y-axis units.
	virtual QString yAxisUnits() const  { return ""; }
};

#endif // VESPERS2DSCANCONFIGURATION_H
