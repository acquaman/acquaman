#ifndef BIOXASUSERCONFIGURATION_H
#define BIOXASUSERCONFIGURATION_H

#include "dataman/CLS/CLSUserConfiguration.h"

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

public slots:

protected:

};

#endif // BIOXASUSERCONFIGURATION_H
