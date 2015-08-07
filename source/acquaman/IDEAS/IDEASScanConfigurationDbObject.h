#ifndef IDEASSCANCONFIGURATIONDBOBJECT_H
#define IDEASSCANCONFIGURATIONDBOBJECT_H

#include "dataman/database/AMDbObject.h"
#include "dataman/AMRegionOfInterest.h"

/// This class is the common database object for all scan configurations for the IDEAS beamline.
class IDEASScanConfigurationDbObject : public AMDbObject
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS Scan Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE IDEASScanConfigurationDbObject(QObject *parent = 0);
	/// Copy constructor.
	IDEASScanConfigurationDbObject(const IDEASScanConfigurationDbObject &original);
	/// Destructor.
	~IDEASScanConfigurationDbObject();


};

#endif // IDEASSCANCONFIGURATIONDBOBJECT_H
