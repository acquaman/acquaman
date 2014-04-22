/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMSignallingGraphicsView.h"

#include <QMouseEvent>

 AMSignallingGraphicsView::~AMSignallingGraphicsView(){}
AMSignallingGraphicsView::AMSignallingGraphicsView(QWidget *parent) :
	QGraphicsView(parent)
{
	doubleClickInProgress_ = false;
	dragStartInProgress_ = false;
	dragInProgress_ = false;
	connect(&singleClickTimer_, SIGNAL(timeout()), this, SLOT(onSingleClickTimerTimeout()));
}

void AMSignallingGraphicsView::mousePressEvent(QMouseEvent *event) {

	doubleClickInProgress_ = false;

	if(dragInProgress_) {	// this could happen if the right mousebutton is pressed while holding the left for a drag
		dragInProgress_ = false;
		emit dragEnded(buttonDownPos_, event->pos());
	}

	// start of a new click/drag/double-click?
	if(event->button() == Qt::LeftButton) {
		dragStartInProgress_ = true;
		buttonDownPos_ = event->pos();
		singleClickTimer_.stop();
	}
	QGraphicsView::mousePressEvent(event);
}

#include <QApplication>
void AMSignallingGraphicsView::mouseReleaseEvent(QMouseEvent *event) {

	if(dragInProgress_) {
		emit dragEnded(buttonDownPos_, event->pos());
	}
	else if(doubleClickInProgress_) {
		if(event->button() == Qt::LeftButton) emit doubleClicked(buttonDownPos_);
	}
	else {
		if(event->button() == Qt::LeftButton) {
			emit clicked(buttonDownPos_);
			singleClickTimer_.start(QApplication::doubleClickInterval()+10);
		}
	}

	doubleClickInProgress_ = false;
	dragStartInProgress_ = false;
	dragInProgress_ = false;
	QGraphicsView::mouseReleaseEvent(event);
}

void AMSignallingGraphicsView::mouseDoubleClickEvent(QMouseEvent *event) {
	doubleClickInProgress_ = true;
	singleClickTimer_.stop();
	QGraphicsView::mouseDoubleClickEvent(event);
}

void AMSignallingGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if(dragStartInProgress_ && (buttonDownPos_-event->pos()).manhattanLength() > QApplication::startDragDistance()) {
		if(event->buttons() & Qt::LeftButton) {
			dragStartInProgress_ = false; // issuing dragStarted()... Only want to do this once per click.
			dragInProgress_ = true;
			emit dragStarted(buttonDownPos_, event->pos());
		}
		else {
			dragStartInProgress_ = false;
		}
	}
	else if(dragInProgress_) {
		if(event->buttons() & Qt::LeftButton) {
			emit dragMoved(buttonDownPos_, event->pos());
		}
		else {
			dragInProgress_ = false;
			emit dragEnded(buttonDownPos_, event->pos());
		}
	}
	QGraphicsView::mouseMoveEvent(event);
}

void AMSignallingGraphicsView::onSingleClickTimerTimeout()
{
	singleClickTimer_.stop();
	emit singleClicked(buttonDownPos_);
}
