#ifndef SXRMBUSERCONFIGURATION_H
#define SXRMBUSERCONFIGURATION_H

#include "dataman/CLS/CLSUserConfiguration.h"

#include "application/SXRMB/SXRMB.h"

/// A class that holds onto the latest experiment configuration and saves it to the database.  This allows it to be seamlessly reloaded from the database upon startup.
class SXRMBUserConfiguration : public CLSUserConfiguration
{
	Q_OBJECT

	Q_PROPERTY(int fluorescenceDetector READ fluorescenceDetector WRITE dbLoadFluorescenceDetector)

	Q_CLASSINFO("AMDbObject_Attributes", "description=SXRMB User Configuration Database Object")

public:
	/// Constructor.  Builds a user configuration.
	Q_INVOKABLE SXRMBUserConfiguration(QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMBUserConfiguration();

	/// Returns the current fluorescence detector choice.
	SXRMB::FluorescenceDetectors fluorescenceDetector() const { return fluorescenceDetector_; }

public slots:
	/// Sets the choice for the fluorescence detector.
	void setFluorescenceDetector(SXRMB::FluorescenceDetectors detector);

protected:
	/// Used for database loading.
	void dbLoadFluorescenceDetector(int detector);

protected:
	/// Fluorescence detector choice.
	SXRMB::FluorescenceDetectors fluorescenceDetector_;
};

#endif // SXRMBUSERCONFIGURATION_H
