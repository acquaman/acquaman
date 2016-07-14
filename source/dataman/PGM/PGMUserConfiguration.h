#ifndef PGMUSERCONFIGURATION_H
#define PGMUSERCONFIGURATION_H

#include "dataman/CLS/CLSUserConfiguration.h"

class PGMUserConfiguration : public CLSUserConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=PGM User Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE PGMUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~PGMUserConfiguration();
};

#endif // PGMUSERCONFIGURATION_H
