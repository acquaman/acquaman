#ifndef BIOXASUSERCONFIGURATION_H
#define BIOXASUSERCONFIGURATION_H

#include "acquaman/CLS/CLSUserConfiguration.h"

/// A class that holds onto the latest experiment configuration and saves it to a database.
class BioXASUserConfiguration : public CLSUserConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=BioXAS User Configuration Database Object")

public:
	/// Constructor. Builds a user configuration.
	Q_INVOKABLE BioXASUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASUserConfiguration();
};

#endif // BIOXASUSERCONFIGURATION_H
