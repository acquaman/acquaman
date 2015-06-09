#ifndef AMGCS2SUPPORT_H
#define AMGCS2SUPPORT_H

#include <QChar>
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
protected:
	AMGCS2Support(){}
};

#endif // AMGCS2SUPPORT_H
