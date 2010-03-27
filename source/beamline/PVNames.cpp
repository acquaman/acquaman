#include "PVNames.h"

QString AMPVNames::ringCurrent;

/// Load settings from disk:
void AMPVNames::load() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "AcquamanProcessVariables");

    // All settings variables are loaded here from disk. Default values must be provided -- they will be used if the particular setting doesn't exist yet.
    // Don't forget to add here if you add new user options.

    // variable = settings.value(key, defaultValue).toType();

    ringCurrent = settings.value("ringCurrent", "PCT1402-01:mA:fbk").toString();


}

/// Save settings to disk:
void AMPVNames::save() {
	QSettings settings(QSettings::IniFormat, QSettings::SystemScope, "Acquaman", "AcquamanProcessVariables");

    // All settings variables are saved here to the user-specific file.
    // Don't forget to add here if you add new user options.

    settings.setValue("ringCurrent", ringCurrent);

}
