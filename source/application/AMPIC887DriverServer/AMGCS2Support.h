#ifndef AMGCS2SUPPORT_H
#define AMGCS2SUPPORT_H

#include <QChar>
#include <QString>
#include "AMGCS2.h"
namespace AMGCS2Support
{
	/*!
	  * Converts between the axis enumerator and its character representation.
	  * \param axis ~ The axis to be converted into a character.
	  */
	inline QChar axisToCharacter(AMGCS2::Axis axis)
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

	/*!
	  * Converts between the character representation of an axis to its corresponding
	  * enumerator value. If the character does not correspond to an axis, the
	  * UnknownAxis value is returned.
	  * \param character ~ The character axis representation whose enumerator value
	  * is to be returned.
	  */
	inline AMGCS2::Axis characterToAxis(const QChar& character)
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

	/*!
	  * Converts from the controller command level enumerator to its string equivalent.
	  * \param commandLevel ~ The command level to stringify.
	  */
	inline QString commandLevelToString(AMGCS2::ControllerCommandLevel commandLevel)
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

	/*!
	  * Converts from the integer command level code to the enumerated equivalent.
	  * Helps in ensuring that command levels other than 0 or 1 return UnknownCommandLevel.
	  * \param ~ The command level code to interpret. 0 = Normal, 1 = Advanced
	  * all other values = UnknownCommandLevel.
	  */
	inline AMGCS2::ControllerCommandLevel intCodeToCommandLevel(int commandLevelCode)
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

	/*!
	  * Converts a data record option enumerator to a human readable string equivalent.
	  * \param dataRecordOption ~ The data record option to stringify.
	  */
	inline QString dataRecordOptionToString(AMGCS2::DataRecordOption dataRecordOption)
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

	/*!
	  * Converts from the integer data record option code to the enumerated equivalent.
	  * Helps in ensuring that data record option codes that are outside of the valid
	  * values are returned as UnknownRecordOption.
	  * \param dataRecordCode ~ The data record code to be converted to its enumerated
	  * equivalent.
	  */
	inline AMGCS2::DataRecordOption intCodeToDataRecordOption(int dataRecordCode)
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

	/*!
	  * Converts from the data record option enumerator to its GCS2 integer code
	  * equivalent.
	  * \param dataRecordOption ~ The record option whose GCS2 code is to be
	  * returned.
	  */
	inline int dataRecordOptionToInt(AMGCS2::DataRecordOption dataRecordOption)
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

	/*!
	  * Converts from the data record source code to the enumerated equivalent.
	  * \param dataSourceCode ~ The character code to be converted to the enumerated
	  * equivalent.
	  */
	inline AMGCS2::DataRecordSource charCodeToDataRecordSource(const QChar& dataSourceCode)
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

	/*!
	  * Converts from the data record source to a string equivalent.
	  * \param recordSource ~ The record source enum to stringify.
	  */
	inline QString dataRecordSourceToString(AMGCS2::DataRecordSource recordSource)
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

	/*!
	  * Converts from the data record source to its GCS2 code equivalent.
	  * \param recordSource ~ The record source whose character code is to be
	  * returned.
	  */
	inline QChar dataRecordSourceToCharCode(AMGCS2::DataRecordSource recordSource)
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

	/*!
	  * Converts from the data record trigger enumerator to a human readable string.
	  * \param recordTrigger ~ The record trigger value to stringify.
	  */
	inline QString dataRecordTriggerToString(AMGCS2::DataRecordTrigger recordTrigger)
	{
		switch(recordTrigger) {
		case AMGCS2::NoRecordTrigger:
			return "No record trigger";
		case AMGCS2::OnNextPositionChange:
			return "Next position change (permanent)";
		case AMGCS2::OnNextCommand_OnceOnly:
			return "On next command (once only)";
		case AMGCS2::TriggerImmediately_OnceOnly:
			return "Immediate (once only)";
		case AMGCS2::OnNextPositionChange_OnceOnly:
			return "Next position change (once only)";
		default:
			return "Unknown record trigger";
		}
	}

	/*!
	  * Converts from the data record trigger enumerator to its GCS2 integer code
	  * equivalent.
	  * \param recordTrigger ~ The record trigger value whose GCS2 integer code
	  * is to be returned.
	  */
	inline int dataRecordTriggerToInt(AMGCS2::DataRecordTrigger recordTrigger)
	{
		switch(recordTrigger) {
		case AMGCS2::NoRecordTrigger:
			return 0;
		case AMGCS2::OnNextPositionChange:
			return 1;
		case AMGCS2::OnNextCommand_OnceOnly:
			return 2;
		case AMGCS2::TriggerImmediately_OnceOnly:
			return 4;
		case AMGCS2::OnNextPositionChange_OnceOnly:
			return 6;
		default:
			return -1;
		}
	}

	/*!
	  * Converts the provided GCS2 integer record trigger code to its enumerated
	  * equivalent. If the provided code is not found in the GCS2 code list for
	  * record trigger, UnknownRecordTrigger will be returned.
	  * \param recordTriggerCode ~ The GCS2 code for the record trigger to convert.
	  */
	inline AMGCS2::DataRecordTrigger intCodeToDataRecordTrigger(int recordTriggerCode)
	{
		switch(recordTriggerCode) {
		case 0:
			return AMGCS2::NoRecordTrigger;
		case 1:
			return AMGCS2::OnNextPositionChange;
		case 2:
			return AMGCS2::OnNextCommand_OnceOnly;
		case 4:
			return AMGCS2::TriggerImmediately_OnceOnly;
		case 6:
			return AMGCS2::OnNextPositionChange_OnceOnly;
		default:
			return AMGCS2::UnknownRecordTrigger;
		}
	}

	/*!
	  * Converts the provided position units string into its enumerated equivalent.
	  * If the provided string does not represent a known position unit,
	  * UnknownPositionUnit will be returned.
	  * \param units ~ A string representation of the units to be converted.
	  */
	inline AMGCS2::PositionUnits stringToPositionUnits(const QString& units)
	{
		if(units == "mm") {
			return AMGCS2::Millimetres;
		} else if (units == "deg") {
			return AMGCS2::Degrees;
		} else {
			return AMGCS2::UnknownPositionUnit;
		}
	}

	/*!
	  * Converts the provided position units enumerator to its string equivalent.
	  * \param units ~ The enumerated values whose string equivalent is to be
	  * returned.
	  */
	inline QString positionUnitsToString(AMGCS2::PositionUnits units)
	{
		switch(units) {
		case AMGCS2::Millimetres:
			return "mm";
		case AMGCS2::Degrees:
			return "deg";
		default:
			return "Unknown Units";
		}
	}
}

#endif // AMGCS2SUPPORT_H
