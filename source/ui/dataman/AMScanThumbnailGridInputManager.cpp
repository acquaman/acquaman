#include "AMScanThumbnailGridInputManager.h"
#include "ui/dataman/AMScanThumbnailGridView.h"

AMScanThumbnailGridInputManager::AMScanThumbnailGridInputManager(QObject *parent)
	: QObject(parent)
{
	//connect( &doubleClickTimer_, SIGNAL(timeout()), this, SLOT(onDoubleClickTimerExpired()) );
	resetInteraction();
	hoverTimer_.setSingleShot(true);
	doubleClickTimer_.setSingleShot(true);
}
#include <QDebug>
void AMScanThumbnailGridInputManager::mouseDownAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons, bool itemIsSelected)
{
	if(mouseButtons & Qt::LeftButton) {
		qDebug() << QString("Left mouse clicked at %1, %2").arg(positionX).arg(positionY);
		// Left mouse button is clicked
		startPositionX_ = positionX;
		startPositionY_ = positionY;

		if(itemIndex != -1) {
			qDebug() << "Mouse is over an item";
			// Mouse is on a item

			if(doubleClickTimer_.isActive()) {
				qDebug() << "Double click timer is running";
				// Double click timer is running

				if(lastClickedItemIndex_ == itemIndex) {
					// Still over the same index on the second click
					qDebug() << "Still over the same index on the second click";
					emit itemDoubleClicked(itemIndex);
					resetInteraction();

				} else {
					// Second click is on a different index
					qDebug() << "Second click is on a different index";
					lastClickedItemIndex_ = itemIndex;
				}
			} else {
				// Double click timer is not running
				qDebug() << "Double click timer is not running";
				lastClickedItemIndex_ = itemIndex;
				startItemSelected_ = itemIsSelected;
			}
		} else {
			// Mouse is not on an item
			qDebug() << "Mouse is not on an item";
			lastClickedItemIndex_ = -1;
			startItemSelected_ = false;
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
		if(!dragInProgress_) {

			// A drag hasn't already begun (we only do stuff when drags are not
			// in progress)

			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));

			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// Current input coordinate is far enough from the start to consider
				// this a drag operation

				if(startItemSelected_) {
					qDebug() << "Drag Begun";
					// Item the mouse was over at the start was selected (need to start a drag)
					dragInProgress_ = true;
					emit dragBegun();
					resetInteraction();
				} else {
					// Item the mouse was over at start was not selected (ie. a rectangle selection operation)
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
		if(lastMouseOverItemIndex_ != itemIndex) {
			// We're now hovering over a different item index
			lastMouseOverItemIndex_ = itemIndex;
			hoverTimer_.start(HOVER_INTERVAL_MS);
			// Reset the lastHoverUpdatePosition
			lastHoverUpdatePositionX_ = -1;

		} else if(itemIndex != -1) {
			// We're still over the same item index

			if(hoverTimer_.isActive()) {
				// The hover timer is still running (ie we're not hovering yet)

			} else {
				// The hover timer has stopped (ie. we're hovering)

				if(lastHoverUpdatePositionX_ == -1) {
					// This is the first time this hover that we've updated the
					// position
					lastHoverUpdatePositionX_ = positionX;
				}

				int travelDistanceX = int(std::abs(double(positionX - lastHoverUpdatePositionX_)));
				if(travelDistanceX > HOVER_TRAVEL_UPDATE_DISTANCE) {
					// We've moved far enough that we need to emit a hover travel update
					emit hoverMove(itemIndex, positionX, positionY);
					lastHoverUpdatePositionX_ = positionX;
				}
			}
		} else {
			lastMouseOverItemIndex_ = itemIndex;
		}
	}
}

void AMScanThumbnailGridInputManager::mouseReleasedAt(int positionX, int positionY, Qt::MouseButtons mouseButtons, Qt::KeyboardModifiers keys)
{
	qDebug() << QString("Mouse release at %1, %2").arg(positionX).arg(positionY);
	if(startPositionX_ == -1 && startPositionY_ == -1) {
		// No interaction is currently being performed. Just return.
		qDebug() << "No interaction is currently being performed";
		return;
	}

	if(!mouseButtons & Qt::LeftButton) {
		// Left button clicked
		qDebug() << "Left button clicked";
		if(dragInProgress_) {
			// Drag has stopped here - We don't accept drags, so nothing doing
			qDebug() << "Drag stopped here";
			resetInteraction();
		} else {
			// No Drag is happening
			qDebug() << "No drag happening";
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle, not a point, selection
				qDebug() << "Must be a rectangle selection";
				if(keys & Qt::ShiftModifier || keys & Qt::ControlModifier) {
					// Shift or Control is held
					qDebug() << "Shift or Control is held";
					QRect finalSelectionRectangle(startPositionX_, startPositionY_, positionX - startPositionX_, positionY - startPositionY_);
					emit selectionRectangleEnded(finalSelectionRectangle.normalized(), QItemSelectionModel::Select);
					resetInteraction();
				} else {
					// No key modifier is held
					qDebug() << "No key modifier held";
					QRect finalSelectionRectangle(startPositionX_, startPositionY_, positionX - startPositionX_, positionY - startPositionY_);
					emit selectionRectangleEnded(finalSelectionRectangle.normalized(), QItemSelectionModel::ClearAndSelect);
					resetInteraction();
				}
			} else {
				// Just a single point click
				qDebug() << "Single point selection";
				if(keys & Qt::ControlModifier) {
					// Control held - toggle the specified index without clearing
					qDebug() << "Control modifier held";
					emit itemSelected(lastClickedItemIndex_, QItemSelectionModel::Toggle);
				} else if(keys & Qt::ShiftModifier) {
					// Shift held - extend the current selection to the last clicked index
					qDebug() << "Shift is held";
					emit selectionExtended(lastClickedItemIndex_);
				} else {
					// Nothing held - clear current selection and select the lastClickedItemIndex_
					qDebug() << "No modifier is held";
					emit itemSelected(lastClickedItemIndex_, QItemSelectionModel::ClearAndSelect);
				}
				resetInteraction();
				doubleClickTimer_.start(DOUBLE_CLICK_INTERVAL_MS);
			}
		}
	} else if(!mouseButtons & Qt::RightButton){
		// Right button clicked

		if(dragInProgress_) {
			// Drag is in progress need to cancel it
		} else {
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > TRAVEL_TOLERANCE ||
					travelDistanceY > TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle selection
				// we need to cancel it
			}
		}
	}

}

void AMScanThumbnailGridInputManager::resetInteraction()
{
	qDebug() << "Interaction reset";
	startPositionX_ = -1;
	startPositionY_ = -1;

	lastUpdatePositionX_ = -1;
	lastUpdatePositionY_ = -1;

	startItemSelected_ = false;

	dragInProgress_ = false;
}

