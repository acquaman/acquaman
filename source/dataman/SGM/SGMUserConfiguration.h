#ifndef SGMUSERCONFIGURATION_H
#define SGMUSERCONFIGURATION_H

#include "dataman/CLS/CLSUserConfiguration.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class SGMUserConfiguration : public CLSUserConfiguration
{
	Q_OBJECT

	Q_CLASSINFO("AMDbObject_Attributes", "description=SGM User Configuration Database Object")

public:
	/// Constructor.
	Q_INVOKABLE SGMUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~SGMUserConfiguration();


public slots:

protected:

protected:
};

#endif // SGMUSERCONFIGURATION_H
