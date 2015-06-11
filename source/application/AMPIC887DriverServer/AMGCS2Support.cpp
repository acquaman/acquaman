#include "AMGCS2Support.h"

QChar AMGCS2Support::axisToCharacter(AMGCS2::Axis axis)
{
	switch (axis) {
	case AMGCS2::XAxis:
		return 'X';
	case AMGCS2::YAxis:
		return 'Y';
	case AMGCS2::ZAxis:
		return 'Z';
	case AMGCS2::UAxis:
		return 'U';
	case AMGCS2::VAxis:
		return 'V';
	case AMGCS2::WAxis:
		return 'W';
	default:
		return QChar();
	}
}

AMGCS2::Axis AMGCS2Support::characterToAxis(const QChar &character)
{
	if(character == 'X') {
		return AMGCS2::XAxis;
	} else if(character == 'Y') {
		return AMGCS2::YAxis;
	} else if(character == 'Z') {
		return AMGCS2::ZAxis;
	} else if(character == 'U') {
		return AMGCS2::UAxis;
	} else if(character == 'V') {
		return AMGCS2::VAxis;
	} else if(character == 'W') {
		return AMGCS2::WAxis;
	}

	return AMGCS2::UnknownAxis;
}

QString AMGCS2Support::commandLevelToString(AMGCS2::ControllerCommandLevel commandLevel)
{
	switch(commandLevel) {
	case AMGCS2::NormalCommandLevel:
		return "Normal command level";
	case AMGCS2::AdvancedCommandLevel:
		return "Advanced command level";
	default:
		return "Unknown command level";
	}
}

AMGCS2::ControllerCommandLevel AMGCS2Support::intCodeToCommandLevel(int commandLevelCode)
{
	switch(commandLevelCode) {
	case 0:
		return AMGCS2::NormalCommandLevel;
	case 1:
		return AMGCS2::AdvancedCommandLevel;
	default:
		return AMGCS2::UnknownCommandLevel;
	}
}
