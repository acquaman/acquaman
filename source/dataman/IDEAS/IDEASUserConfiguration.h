#ifndef IDEASUSERCONFIGURATION_H
#define IDEASUSERCONFIGURATION_H

#include "dataman/CLS/CLSUserConfiguration.h"

#include "application/IDEAS/IDEAS.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.
class IDEASUserConfiguration : public CLSUserConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE dbLoadFluorescenceDetector)

	Q_CLASSINFO("AMDbObject_Attributes", "description=IDEAS User Configuration Database Object")

public:
	/// Constructor.  Builds a user configuration.
	Q_INVOKABLE IDEASUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~IDEASUserConfiguration();

	/// Returns the current fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }

public slots:
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(IDEAS::FluorescenceDetectors detector);

protected:
	/// Used for database loading.
	void dbLoadFluorescenceDetector(int detector);

protected:
	/// Fluorescence detector choice.
	IDEAS::FluorescenceDetectors fluorescenceDetector_;
};

#endif // IDEASUSERCONFIGURATION_H
