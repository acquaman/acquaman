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

QString AMGCS2Support::dataRecordOptionToString(AMGCS2::DataRecordOption dataRecordOption)
{
	switch(dataRecordOption) {
	case AMGCS2::NothingIsRecorded:
		return "Nothing is recorded";
	case AMGCS2::CommandedPositionOfAxis:
		return "Commanded position of axis";
	case AMGCS2::RealPositionOfAxis:
		return "Real position of axis";
	case AMGCS2::PositionErrorOfAxis:
		return "Position error of axis";
	case AMGCS2::MeasurementTime:
		return "Measurement time";
	case AMGCS2::CommandedVelocityOfAxis:
		return "Commanded velocity of axis";
	case AMGCS2::CommandedAccelerationOfAxis:
		return "Commanded acceleration of axis";
	case AMGCS2::RealVelocityOfAxis:
		return "Real velocity of axis";
	case AMGCS2::MotorOutputOfAxis:
		return "Motor output of axis";
	case AMGCS2::CurrentIntegratedPositionErrorOfAxis:
		return "Current integrated position error of axis";
	case AMGCS2::StatusRegisterOfAxis:
		return "Status register of axis";
	default:
		return "Unknown data record option";
	}
}

AMGCS2::DataRecordOption AMGCS2Support::intCodeToDataRecordOption(int dataRecordCode)
{
	switch(dataRecordCode) {
	case 0:
		return AMGCS2::NothingIsRecorded;
	case 1:
		return AMGCS2::CommandedPositionOfAxis;
	case 2:
		return AMGCS2::RealPositionOfAxis;
	case 3:
		return AMGCS2::PositionErrorOfAxis;
	case 8:
		return AMGCS2::MeasurementTime;
	case 70:
		return AMGCS2::CommandedVelocityOfAxis;
	case 71:
		return AMGCS2::CommandedAccelerationOfAxis;
	case 72:
		return AMGCS2::RealVelocityOfAxis;
	case 73:
		return AMGCS2::MotorOutputOfAxis;
	case 76:
		return AMGCS2::CurrentIntegratedPositionErrorOfAxis;
	case 80:
		return AMGCS2::StatusRegisterOfAxis;
	default:
		return AMGCS2::UnknownRecordOption;
	}
}


int AMGCS2Support::dataRecordOptionToInt(AMGCS2::DataRecordOption dataRecordOption)
{
	switch(dataRecordOption) {
	case AMGCS2::NothingIsRecorded:
		return 0;
	case AMGCS2::CommandedPositionOfAxis:
		return 1;
	case AMGCS2::RealPositionOfAxis:
		return 2;
	case AMGCS2::PositionErrorOfAxis:
		return 3;
	case AMGCS2::MeasurementTime:
		return 8;
	case AMGCS2::CommandedVelocityOfAxis:
		return 70;
	case AMGCS2::CommandedAccelerationOfAxis:
		return 71;
	case AMGCS2::RealVelocityOfAxis:
		return 72;
	case AMGCS2::MotorOutputOfAxis:
		return 73;
	case AMGCS2::CurrentIntegratedPositionErrorOfAxis:
		return 76;
	case AMGCS2::StatusRegisterOfAxis:
		return 80;
	default:
		return -1;
	}
}

AMGCS2::DataRecordSource AMGCS2Support::charCodeToDataRecordSource(const QChar &dataSourceCode)
{
	if(dataSourceCode == 'X') {
		return AMGCS2::RecordXAxis;
	} else if(dataSourceCode == 'Y') {
		return AMGCS2::RecordYAxis;
	} else if(dataSourceCode == 'Z') {
		return AMGCS2::RecordZAxis;
	} else if(dataSourceCode == 'U') {
		return AMGCS2::RecordUAxis;
	} else if(dataSourceCode == 'V') {
		return AMGCS2::RecordVAxis;
	} else if(dataSourceCode == 'W') {
		return AMGCS2::RecordWAxis;
	} else if(dataSourceCode == '1') {
		return AMGCS2::RecordSystemWide;
	} else if (dataSourceCode == '0') {
		return AMGCS2::NoRecordSource;
	} else {
		return AMGCS2::UnknownDataRecordSource;
	}
}

QString AMGCS2Support::dataRecordSourceToString(AMGCS2::DataRecordSource recordSource)
{
	switch(recordSource) {
	case AMGCS2::RecordXAxis:
		return "X";
	case AMGCS2::RecordYAxis:
		return "Y";
	case AMGCS2::RecordZAxis:
		return "Z";
	case AMGCS2::RecordUAxis:
		return "U";
	case AMGCS2::RecordVAxis:
		return "V";
	case AMGCS2::RecordWAxis:
		return "W";
	case AMGCS2::RecordSystemWide:
		return "System";
	case AMGCS2::NoRecordSource:
		return "None";
	default:
		return "Unknown";
	}
}

QChar AMGCS2Support::dataRecordSourceToCharCode(AMGCS2::DataRecordSource recordSource)
{
	switch(recordSource) {
	case AMGCS2::RecordXAxis:
		return 'X';
	case AMGCS2::RecordYAxis:
		return 'Y';
	case AMGCS2::RecordZAxis:
		return 'Z';
	case AMGCS2::RecordUAxis:
		return 'U';
	case AMGCS2::RecordVAxis:
		return 'V';
	case AMGCS2::RecordWAxis:
		return 'W';
	case AMGCS2::RecordSystemWide:
		return '1';
	case AMGCS2::NoRecordSource:
		return '0';
	default:
		return 'E';
	}
}
