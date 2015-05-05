#include "AMScanThumbnailGridInputManager.h"
#include "ui/dataman/AMScanThumbnailGridView.h"

#include <QDebug>
AMScanThumbnailGridInputManager::AMScanThumbnailGridInputManager(QObject *parent)
	: QObject(parent)
{
	connect( &doubleClickTimer_, SIGNAL(timeout()), this, SLOT(onDoubleClickTimerExpired()) );
	resetInteraction();
	hoverTimer_.setSingleShot(true);
}

void AMScanThumbnailGridInputManager::mouseDownAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons)
{
	qDebug() << "Mouse Down";
	if(mouseButtons & Qt::LeftButton) {
		// Left mouse button is clicked
		qDebug() << "Left Mouse button down";
		startPositionX_ = positionX;
		startPositionY_ = positionY;

		if(itemIndex != -1) {
			// Mouse is on a item
			qDebug() << "Mouse is over an item";

			if(doubleClickTimer_.isActive()) {
				// Double click timer is running
				qDebug() << "Double click timer is running";

				if(lastClickedItemIndex_ == itemIndex) {
					// Still over the same index on the second click
					qDebug() << "Still over the same index as last click";

					emit itemDoubleClicked(itemIndex);
					qDebug() << QString("Double click item with index %1").arg(lastClickedItemIndex_);
					resetInteraction();

				} else {
					// Second click is on a different index
					qDebug() << "Second click on a different index";
					lastClickedItemIndex_ = itemIndex;
				}
			} else {
				// Double click timer is not running
				qDebug() << "Double click timer not running";
				lastClickedItemIndex_ = itemIndex;
			}
		} else {
			// Mouse is not on an item
			qDebug() << "Mouse is not over an item";
			lastClickedItemIndex_ = -1;
			doubleClickTimer_.stop();
		}
	}
}

void AMScanThumbnailGridInputManager::mouseMovedTo(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons)
{
	lastUpdatePositionX_ = positionX;
	lastUpdatePositionY_ = positionY;

	if(mouseButtons & Qt::LeftButton) {
		// Left mouse button is held
		qDebug() << "Left button held";
		if(!dragInProgress_) {

			// A drag hasn't already begun (we only do stuff when drags are not
			// in progress)

			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			qDebug() << QString ("travelDistanceX = %1").arg(travelDistanceX);
			qDebug() << QString ("travelDistanceY = %1").arg(travelDistanceY);
			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// Current input coordinate is far enough from the start to consider
				// this a drag operation

				if(lastClickedItemIndex_ != -1) {
					// Mouse was over an item at start of interaction (ie this is a drag)
					qDebug() << QString("Drag begun with item %1").arg(itemIndex);
					dragInProgress_ = true;
					emit dragBegun(itemIndex);
				} else {
					// Mouse was not over an item (ie. a rectangle selection operation)
					emit selectionRectangleChanged(QRect(startPositionX_,
														 startPositionY_,
														 lastUpdatePositionX_ - startPositionX_,
														 lastUpdatePositionY_ - startPositionY_)
												   );
				}
			}
		}
	} else if (mouseButtons & Qt::RightButton) {
		// Right mouse button is held

		// Do nothing

	} else {
		// No mouse button (of interest) is held
		if(itemIndex != -1 && lastMouseOverItemIndex_ != itemIndex) {
			// We're now hovering over a different item index
			qDebug() << QString("Now hovering over item with index %1").arg(itemIndex);
			lastMouseOverItemIndex_ = itemIndex;
			hoverTimer_.start(HOVER_INTERVAL_MS);
			if(hoverTimer_.isActive()) {
				qDebug() << "RUNNING!";
			} else {
				qDebug() << "NOT RUNNING!";
			}
		} else if(itemIndex != -1) {
			// We're still over the same item index

			if(hoverTimer_.isActive()) {
				// The hover timer is still running (ie we're not hovering yet)

			} else {
				// The hover timer has stopped (ie. we're hovering)

				/////////////////////////////////////////////////////////
				// TODO: Figure out when to update tavel delta x and y
				/////////////////////////////////////////////////////////
			}
		} else {
			lastMouseOverItemIndex_ = itemIndex;
		}
	}
}

void AMScanThumbnailGridInputManager::mouseReleasedAt(int positionX, int positionY, Qt::MouseButtons mouseButtons, Qt::KeyboardModifiers keys)
{
	if(startPositionX_ == -1 && startPositionY_ == -1) {
		// No interaction is currently being performed. Just return.
		return;
	}

	if(!mouseButtons & Qt::LeftButton) {
		// Left button clicked
		qDebug() << "Left button released";
		if(dragInProgress_) {
			// Drag has stopped here - We don't accept drags, so nothing doing
			qDebug() << "Drag was happening";
			resetInteraction();
		} else {
			// No Drag is happening
			qDebug() << "No drag was happening";
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle, not a point, selection
				if(keys & Qt::ShiftModifier || keys & Qt::ControlModifier) {
					// Shift or Control is held
					emit selectionRectangleEnded(QItemSelectionModel::Select);
					qDebug() << QString("Selection Rectangle Ended with shift or control held");
					resetInteraction();
				} else {
					emit selectionRectangleEnded(QItemSelectionModel::ClearAndSelect);
					qDebug() << QString("Selection Rectangle Ended");
					resetInteraction();
				}
			} else {
				// Just a single point click
				qDebug() << QString("Starting double click timer");
				keys_ = keys;
				doubleClickTimer_.start(DOUBLE_CLICK_INTERVAL_MS);
			}
		}
	} else if(!mouseButtons & Qt::RightButton){
		// Right button clicked
		qDebug() << "Right button released";
		if(dragInProgress_) {
			// Drag is in progress need to cancel it
		} else {
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle, not a point, selection
				// we need to cancel it
			}
		}
	}

}

void AMScanThumbnailGridInputManager::onDoubleClickTimerExpired()
{
	qDebug() << QString("Emitting itemSelected at index %1").arg(lastClickedItemIndex_);
	if(keys_ & Qt::ControlModifier) {
		// Control held - toggle the specified index without clearing
		emit itemSelected(lastClickedItemIndex_, QItemSelectionModel::Toggle);
	} else if(keys_ & Qt::ShiftModifier) {
		// Shift held - extend the current selection to the last clicked index
		emit selectionExtended(lastClickedItemIndex_);
	} else {
		// Nothing held - clear current selection and select the lastClickedItemIndex_
		emit itemSelected(lastClickedItemIndex_, QItemSelectionModel::ClearAndSelect);
	}
	resetInteraction();
}

void AMScanThumbnailGridInputManager::resetInteraction() {
	qDebug() <<"Interaction reset!";
	startPositionX_ = -1;
	startPositionY_ = -1;

	lastUpdatePositionX_ = -1;
	lastUpdatePositionY_ = -1;

	lastClickedItemIndex_ = -1;
	lastMouseOverItemIndex_ = -1;

	doubleClickTimer_.stop();

	dragInProgress_ = false;
	keys_ = 0;
}

