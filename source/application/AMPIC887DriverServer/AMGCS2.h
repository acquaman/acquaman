#ifndef AMGCS2_H
#define AMGCS2_H

#define LINEAR_AXIS_COUNT 3
#define ROTATION_AXIS_COUNT 3
#define AXIS_COUNT (LINEAR_AXIS_COUNT + ROTATION_AXIS_COUNT)
#define RECORD_TABLE_COUNT 16

#include <QChar>
/*!
  * Namespace which contains all the enumerators for use with the GCS2.0 syntax
  * provided by the PI C887.11 controllers.
  */
namespace AMGCS2 {

	/***************
	  * Enums
	  ****************************/

	/*!
	  * Enumerates all the hexapod axes controlled by a C887.11 controller.
	  */
	enum Axis {
		UnknownAxis = -1,
		XAxis = 0,
		YAxis,
		ZAxis,
		UAxis,
		VAxis,
		WAxis
	};

	/*!
	  * Enumerates all the axes movement statuses of the hexapod controlled by
	  * a C887.11 controller.
	  */
	enum AxisMovementStatus {
		UnknownAxisMovementStatus = 1,
		XAxisIsMoving = 2,
		YAxisIsMoving = 4,
		ZAxisIsMoving = 8,
		UAxisIsMoving = 16,
		VAxisIsMoving = 32,
		WAxisIsMoving = 64
	};

	Q_DECLARE_FLAGS(AxisMovementStatuses, AxisMovementStatus)

	/*!
	  * Enumerates the different command levels of the C887.11 controller.
	  */
	enum ControllerCommandLevel {
		UnknownCommandLevel = -1,
		NormalCommandLevel = 0,
		AdvancedCommandLevel = 1
	};

	/*!
	  * Enumerates the different data record sources of a C887.11 controller.
	  */
	enum DataRecordSource {
		UnknownDataRecordSource = -1,
		RecordXAxis = 0,
		RecordYAxis = 1,
		RecordZAxis = 2,
		RecordUAxis = 3,
		RecordVAxis = 4,
		RecordWAxis = 5,
		RecordSystemWide = 6,
		NoRecordSource = 7
	};

	/*!
	  * Enumerates all the different data recording options for an axis on a C887.11
	  * controller.
	  */
	enum DataRecordOption {
		UnknownRecordOption = -1,
		NothingIsRecorded = 0,
		CommandedPositionOfAxis = 1,
		RealPositionOfAxis = 2,
		PositionErrorOfAxis = 3,
		MeasurementTime = 8,
		CommandedVelocityOfAxis = 70,
		CommandedAccelerationOfAxis = 71,
		RealVelocityOfAxis = 72,
		MotorOutputOfAxis = 73,
		CurrentIntegratedPositionErrorOfAxis = 76,
		StatusRegisterOfAxis = 80
	};

	/*!
	  * Enumerates all the different data recording triggers for a record table
	  * on a C887.11 controller.
	  */
	enum DataRecordTrigger {
		UnknownRecordTrigger = -1,
		NoRecordTrigger = 0,
		OnNextPositionChange = 1,
		OnNextCommand_OnceOnly = 2,
		TriggerImmediately_OnceOnly = 4,
		OnNextPositionChange_OnceOnly = 6
	};

	/*!
	  * Enumerates all the different valid position units of the C887.11 controller.
	  */
	enum PositionUnits {
		UnknownPositionUnit = -1,
		Millimetres,
		Degrees
	};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(AMGCS2::AxisMovementStatuses)
#endif // AMGCS2_H
