#include "AMScanThumbnailGridInputManager.h"
#include "ui/dataman/AMScanThumbnailGridView.h"

#include <QDebug>
AMScanThumbnailGridInputManager::AMScanThumbnailGridInputManager(AMScanThumbnailGridView *view,
																 QObject *parent) :
	QObject(parent) {
	view_ = view;


}

void AMScanThumbnailGridInputManager::mouseDownAt(int itemIndex,
												  int positionX,
												  int positionY,
												  Qt::MouseButtons mouseButtons) {

	if(mouseButtons & Qt::LeftButton) {
		// Left mouse button is clicked
		qDebug() << QString("Left Mouse button clicked");
		startPositionX_ = positionX;
		startPositionY_ = positionY;

		if(itemIndex != -1) {
			// Mouse is on a item
			qDebug() << QString("Mouse is over an item");

			if(doubleClickTimer_.isActive()) {
				// Double click timer is running
				qDebug() << QString("Double click timer is running (ie this is a double click)");

				if(lastClickedItemIndex_ == itemIndex) {
					// Still over the same index on the second click
					qDebug() << QString("Mouse is still over the same index as last time");

					emit itemDoubleClicked(itemIndex);
					resetInteraction();

				} else {
					// Second click is on a different index
					qDebug() << QString("Second click is on a different index");

					lastClickedItemIndex_ = itemIndex;
				}
			} else {
				// Double click timer is not running
				qDebug() << QString("Double click timer is not running (ie. this is not a double click)");
				doubleClickTimer_.start();
				lastClickedItemIndex_ = itemIndex;
			}
		} else {
			// Mouse is not on an item
			qDebug() << QString("Mouse is not over an item");
			lastClickedItemIndex_ = -1;
			doubleClickTimer_.stop();
		}
	}
}

void AMScanThumbnailGridInputManager::mouseMovedTo(int itemIndex,
												   int positionX,
												   int positionY,
												   Qt::MouseButtons mouseButtons) {
	lastUpdatePositionX_ = positionX;
	lastUpdatePositionY_ = positionY;

	if(mouseButtons & Qt::LeftButton) {
		// Left mouse button is held

		if(!dragInProgress_) {

			// A drag hasn't already begun (we only do stuff when drags are not
			// in progress)

			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > RECTANGLE_SELECTION_TRAVEL_TOLERANCE ||
					travelDistanceY > RECTANGLE_SELECTION_TRAVEL_TOLERANCE) {
				// Current input coordinate is far enough from the start to consider
				// this a rectangle selection

				if(itemIndex != -1) {
					// Mouse is over an item (ie. a drag operation)
					dragInProgress_ = true;
					emit dragBegun(itemIndex);
				} else {
					// Mouse is not over an item (ie. a rectangle selection operation)
					emit selectionRectangleChanged(QRect(startPositionX_,
														 startPositionY_,
														 lastUpdatePositionX_ - startPositionX_,
														 lastUpdatePositionY_ - startPositionY_));
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
			hoverTimer_.start();
		} else {
			// We're still over the same item index

			if(hoverTimer_.isActive()) {
				// The hover timer is still running (ie we're not hovering yet)

			} else {
				// The hover timer has stopped (ie. we're hovering)

				// TODO:
				// Figure out some way of determining if we're travelled far
				// enough forward or backwards to emit a thumbnailIncrement/
				// thumbnailDecrement signal
				/////////////////////////////////////////////////////
			}
		}
	}
}

void AMScanThumbnailGridInputManager::mouseReleasedAt(int itemIndex,
													  int positionX,
													  int positionY,
													  Qt::MouseButtons mouseButtons) {
	if(mouseButtons & Qt::LeftButton) {
		// Left button clicked
		if(dragInProgress_) {
			// Drag has stopped here - We don't accept drags, so nothing doing
		} else {
			// No Drag is happening
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > RECTANGLE_SELECTION_TRAVEL_TOLERANCE ||
					travelDistanceY > RECTANGLE_SELECTION_TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle, not a point, selection
				if(mouseButtons & Qt::ShiftModifier || mouseButtons & Qt::ControlModifier) {
					// Shift or Control is held
					emit selectionRectangleEnded(QItemSelectionModel.Select);
					resetInteraction();
				} else {
					emit selectionRectangleEnded(QItemSelectionModel.ClearAndSelect);
					resetInteraction();
				}
			} else {
				// Just a single point click
			}
		}
	} else {
		// Right button clicked
		if(dragInProgress_) {
			// Drag is in progress need to cancel it
		} else {
			int travelDistanceX = int(std::abs(double(positionX - startPositionX_)));
			int travelDistanceY = int(std::abs(double(positionY - startPositionY_)));
			if(travelDistanceX > RECTANGLE_SELECTION_TRAVEL_TOLERANCE ||
					travelDistanceY > RECTANGLE_SELECTION_TRAVEL_TOLERANCE) {
				// We've moved far enough that this must be a rectangle, not a point, selection
				// we need to cancel it
			}
		}
	}

}


void AMScanThumbnailGridInputManager::resetInteraction() {
	startPositionX_ = -1;
	startPositionY_ = -1;

	lastUpdatePositionX_ = -1;
	lastUpdatePositionY_ = -1;

	lastClickedItemIndex_ = -1;
	lastMouseOverItemIndex_ = -1;

	doubleClickTimer_.stop();

	dragInProgress_ = false;
}
