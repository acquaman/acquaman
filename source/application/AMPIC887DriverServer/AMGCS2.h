#ifndef AMGCS2_H
#define AMGCS2_H

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
		YAxis = 1,
		ZAxis = 2,
		UAxis = 3,
		VAxis = 4,
		WAxis = 5
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
}
#endif // AMGCS2_H
