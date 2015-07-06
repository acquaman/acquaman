#ifndef AMPIC887CONTROLLERSTATE_H
#define AMPIC887CONTROLLERSTATE_H

#include "AMPIC887HexapodState.h"
#include <QString>
/*!
  * A class which holds the current state of a PI C887.11 Controller. This class
  * caches the data held within the controller to decrease the number of command
  * calls to the controller when they are not necessary. In doing so it makes the
  * assumption that no other source of commands (which might change these values)
  * exists to the controller.
  */
class AMPIC887ControllerState
{
public:

	/*!
	  * Creates a new instance of a PI C887 Controller State with no currently
	  * initialized data.
	  */
    AMPIC887ControllerState();

	/*!
	  * Frees the resources owned by the controller state.
	  */
	~AMPIC887ControllerState();

	/*!
	  * The current state of the hexapod which the controller represented by this
	  * state is controlling.
	  */
	AMPIC887HexapodState* hexapodState() const;

	/*!
	  * Whether all the data stored within this controller state is initialized.
	  * \return True if the contained hexapod is initialized, false otherwise.
	  */
	bool isAllInitialized() const;

	/*!
	  * Returns a string which describes the current status of the controller's
	  * data.
	  */
	QString statusString() const;
protected:

	AMPIC887HexapodState* hexapodState_;
};

#endif // AMPIC887CONTROLLERSTATE_H
