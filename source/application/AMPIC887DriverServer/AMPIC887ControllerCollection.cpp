#include "AMPIC887ControllerCollection.h"

AMPIC887ControllerCollection::AMPIC887ControllerCollection()
{
	activeController_ = 0;
	controllers_ = new QList<AMPIC887Controller*>();
}

AMPIC887ControllerCollection::~AMPIC887ControllerCollection()
{
	for (int iController = 0, controllerCount = controllers_->count();
		 iController < controllerCount;
		 ++iController) {

		delete controllers_->at(iController);
	}

	controllers_->clear();
	delete controllers_;
}

void AMPIC887ControllerCollection::append(AMPIC887Controller *controller)
{
	controllers_->append(controller);
	if(controllers_->count() == 1) {
		activeController_ = controller;
	}
}

AMPIC887Controller * AMPIC887ControllerCollection::at(int index) const
{
	return controllers_->at(index);
}

void AMPIC887ControllerCollection::setActiveController(AMPIC887Controller *activeController)
{
	if(controllers_->contains(activeController)) {
		activeController_ = activeController;
	}
}

void AMPIC887ControllerCollection::setActiveController(const QString& controllerName)
{
	if(activeController_ && activeController_->name() == controllerName) {
		return;
	}

	bool foundController = false;
	for(int iController = 0, controllerCount = controllers_->count();
		iController < controllerCount && !foundController;
		++iController) {

		AMPIC887Controller* currentController = controllers_->at(iController);
		if(currentController->name() == controllerName) {
			activeController_ = currentController;
			foundController = true;
		}
	}
}

AMPIC887Controller * AMPIC887ControllerCollection::activeController() const
{
	return activeController_;
}

int AMPIC887ControllerCollection::count() const
{
	return controllers_->count();
}

QString AMPIC887ControllerCollection::connectionStatuses() const
{
	if(controllers_->isEmpty()) {
		return "No connected controllers";
	}

	QString returnString;

	for(int iController = 0, controllerCount = controllers_->count();
		iController < controllerCount;
		++iController) {

		returnString.append(controllers_->at(iController)->status());
		returnString.append("\n");
	}

	returnString.append("\n");
	returnString.append("Active Controller:\n");
	if(!activeController_) {
		returnString.append("No controller is currently active!");
	} else {
		returnString.append(activeController_->status());
	}


	return returnString;
}
