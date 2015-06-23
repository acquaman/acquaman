#ifndef AMGCS2SUPPORT_H
#define AMGCS2SUPPORT_H

#include <QChar>
#include <QString>
#include "AMGCS2.h"
class AMGCS2Support
{
public:
	/*!
	  * Converts between the axis enumerator and its character representation.
	  * \param axis ~ The axis to be converted into a character.
	  */
	static QChar axisToCharacter(AMGCS2::Axis axis);

	/*!
	  * Converts between the character representation of an axis to its corresponding
	  * enumerator value. If the character does not correspond to an axis, the
	  * UnknownAxis value is returned.
	  * \param character ~ The character axis representation whose enumerator value
	  * is to be returned.
	  */
	static AMGCS2::Axis characterToAxis(const QChar& character);

	/*!
	  * Converts from the controller command level enumerator to its string equivalent.
	  * \param commandLevel ~ The command level to stringify.
	  */
	static QString commandLevelToString(AMGCS2::ControllerCommandLevel commandLevel);

	/*!
	  * Converts from the integer command level code to the enumerated equivalent.
	  * Helps in ensuring that command levels other than 0 or 1 return UnknownCommandLevel.
	  * \param ~ The command level code to interpret. 0 = Normal, 1 = Advanced
	  * all other values = UnknownCommandLevel.
	  */
	static AMGCS2::ControllerCommandLevel intCodeToCommandLevel(int commandLevelCode);

	/*!
	  * Converts a data record option enumerator to a human readable string equivalent.
	  * \param dataRecordOption ~ The data record option to stringify.
	  */
	static QString dataRecordOptionToString(AMGCS2::DataRecordOption dataRecordOption);

	/*!
	  * Converts from the integer data record option code to the enumerated equivalent.
	  * Helps in ensuring that data record option codes that are outside of the valid
	  * values are returned as UnknownRecordOption.
	  * \param dataRecordCode ~ The data record code to be converted to its enumerated
	  * equivalent.
	  */
	static AMGCS2::DataRecordOption intCodeToDataRecordOption(int dataRecordCode);

	/*!
	  * Converts from the data record option enumerator to its GCS2 integer code
	  * equivalent.
	  * \param dataRecordOption ~ The record option whose GCS2 code is to be
	  * returned.
	  */
	static int dataRecordOptionToInt(AMGCS2::DataRecordOption dataRecordOption);

	/*!
	  * Converts from the data record source code to the enumerated equivalent.
	  * \param dataSourceCode ~ The character code to be converted to the enumerated
	  * equivalent.
	  */
	static AMGCS2::DataRecordSource charCodeToDataRecordSource(const QChar& dataSourceCode);

	/*!
	  * Converts from the data record source to a string equivalent.
	  * \param recordSource ~ The record source enum to stringify.
	  */
	static QString dataRecordSourceToString(AMGCS2::DataRecordSource recordSource);

	/*!
	  * Converts from the data record source to its GCS2 code equivalent.
	  * \param recordSource ~ The record source whose character code is to be
	  * returned.
	  */
	static QChar dataRecordSourceToCharCode(AMGCS2::DataRecordSource recordSource);

	/*!
	  * Converts from the data record trigger enumerator to a human readable string.
	  * \param recordTrigger ~ The record trigger value to stringify.
	  */
	static QString dataRecordTriggerToString(AMGCS2::DataRecordTrigger recordTrigger);

	/*!
	  * Converts from the data record trigger enumerator to its GCS2 integer code
	  * equivalent.
	  * \param recordTrigger ~ The record trigger value whose GCS2 integer code
	  * is to be returned.
	  */
	static int dataRecordTriggerToInt(AMGCS2::DataRecordTrigger recordTrigger);

	/*!
	  * Converts the provided GCS2 integer record trigger code to its enumerated
	  * equivalent. If the provided code is not found in the GCS2 code list for
	  * record trigger, UnknownRecordTrigger will be returned.
	  * \param recordTriggerCode ~ The GCS2 code for the record trigger to convert.
	  */
	static AMGCS2::DataRecordTrigger intCodeToDataRecordTrigger(int recordTriggerCode);

	/*!
	  * Converts the provided position units string into its enumerated equivalent.
	  * If the provided string does not represent a known position unit,
	  * UnknownPositionUnit will be returned.
	  * \param units ~ A string representation of the units to be converted.
	  */
	static AMGCS2::PositionUnits stringToPositionUnits(const QString& units);

	/*!
	  * Converts the provided position units enumerator to its string equivalent.
	  * \param units ~ The enumerated values whose string equivalent is to be
	  * returned.
	  */
	static QString positionUnitsToString(AMGCS2::PositionUnits units);
protected:
	/*!
	  * Static class, so we protect the constructor to prevent initialization.
	  */
	AMGCS2Support(){}
};

#endif // AMGCS2SUPPORT_H
