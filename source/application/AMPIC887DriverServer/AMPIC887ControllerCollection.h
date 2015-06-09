#ifndef AMPIC887CONTROLLERCOLLECTION_H
#define AMPIC887CONTROLLERCOLLECTION_H

#include "AMPIC887Controller.h"
#include <QList>
/*!
  * A class representing a collection of AMPIC887Controllers
  */
class AMPIC887ControllerCollection
{
public:
	/*!
	  * Creates an empty instance of a controller collection.
	  */
    AMPIC887ControllerCollection();

	/*!
	  * Destructor for a controller collection. Frees all the resources related
	  * to the contained controllers.
	  */
	~AMPIC887ControllerCollection();

	/*!
	  * Appends the provided controller to the end of the collection.
	  * NOTE: The collection takes ownership of the provided controller.
	  * \param controller ~ The controller to append to the controller collection.
	  */
	void append(AMPIC887Controller* controller);

	/*!
	  * The controller at the supplied index in the collection.
	  * \param index ~ The index position of the controller to return.
	  */
	AMPIC887Controller* at(int index) const;

	/*!
	  * Sets the active controller to be the provided active controller, if it
	  * is found in the collection. If not the active controller remains as it
	  * was prior to the call.
	  * \param activeController ~ The controller which will become the active
	  * controller.
	  */
	void setActiveController(AMPIC887Controller* activeController);

	/*!
	  * Sets the active controller to be the controller within the collection
	  * with the provided controllerName. If no controller is found with a matching
	  * controller name the active controller remains as it was prior to the call.
	  * If more than one controller exists in the collection with the same name,
	  * the first located matching controller will be made the active controller.
	  * \param controllerName ~ The name of the controller to make the active controller.
	  */
	void setActiveController(const QString& controllerName);

	/*!
	  * The current active controller. The active controller will receive the
	  * commands issued manually into the console.
	  */
	AMPIC887Controller* activeController() const;

	/*!
	  * The number of contained controllers within the collection.
	  */
	int count() const;

	/*!
	  * Returns a description of all the contained controllers statuses, plus
	  * the status of the current active controller.
	  */
	QString connectionStatuses() const;
protected:
	QList<AMPIC887Controller*>* controllers_;
	AMPIC887Controller* activeController_;
};

#endif // AMPIC887CONTROLLERCOLLECTION_H
