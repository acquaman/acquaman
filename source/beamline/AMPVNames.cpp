#include "AMPVNames.h"

/// forward- and reverse-enabled lookup of keyValue/keyValue pairs
AMBiHash<QString, QString> AMPVNames::d_;


/// Load settings from disk.  You must specify all PVs here, along with default values to use if they're not found in the config file.  However, changes to the PV names can be done through the config file without rebuilding Acquaman.
void AMPVNames::load() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "AcquamanProcessVariables");

    // All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.


	d_.set("ringCurrent", settings.value("ringCurrent", "PCT1402-01:mA:fbk").toString());
	d_.set("beamlineEnergy", settings.value("beamlineEnergy", "BL1611-ID-1:Energy").toString());


}

/// Save settings to disk:
void AMPVNames::save() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "AcquamanProcessVariables");

	// All settings variables are saved here to the system-specific file.

	QHashIterator<QString, QString> i = d_.iterator();
	while(i.hasNext()) {
		i.next();
		settings.setValue(i.key(), i.value());
	}

}
