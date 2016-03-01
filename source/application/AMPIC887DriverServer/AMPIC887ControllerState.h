#ifndef AMPIC887CONTROLLERSTATE_H
#define AMPIC887CONTROLLERSTATE_H

#include "AMPIC887HexapodState.h"
#include "AMPIC887DataRecorderState.h"
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
	  * The current state of the controller's data recorder.
	  */
	AMPIC887DataRecorderState* dataRecorderState() const;

	/*!
	  * Initialized the data local to this controller. In order for isAllInitialized
	  * to return true the contained dataRecorderState and hexapodState must also
	  * been initialized.
	  */
	void initialize(AMGCS2::ControllerCommandLevel commandLevel,
					const QString& availableParameters,
					const QString& identification);

	/*!
	  * Whether all the data stored within this controller state is valid for
	  * safe controller operation.
	  * \return True if the contained hexapod is initialized, false otherwise.
	  */
	bool isDataValid() const;

	/*!
	  * Returns a string which describes the current status of the controller's
	  * data.
	  */
	QString statusString() const;

	/*!
	  * A string which contains the available parameters used to determine the
	  * behaviours of the controller.
	  */
	QString availableParameters() const;

	/*!
	  * A string with the model and serial number of the controller.
	  */
	QString identificationString() const;

	/*!
	  * The controller command level stored within this state.
	  */
	AMGCS2::ControllerCommandLevel currentCommandLevel() const;

	/*!
	  * Sets the controller command level stored within this state.
	  */
	void setCurrentCommandLevel(AMGCS2::ControllerCommandLevel currentCommandLevel);
protected:

	AMPIC887HexapodState* hexapodState_;
	AMPIC887DataRecorderState* dataRecorderState_;

	QString availableParameters_;
	QString identificationString_;
	AMGCS2::ControllerCommandLevel currentCommandLevel_;

};

#endif // AMPIC887CONTROLLERSTATE_H
