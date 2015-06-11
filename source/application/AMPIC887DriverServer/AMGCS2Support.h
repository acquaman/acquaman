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

protected:
	/*!
	  * Static class, so we protect the constructor to prevent initialization.
	  */
	AMGCS2Support(){}
};

#endif // AMGCS2SUPPORT_H
