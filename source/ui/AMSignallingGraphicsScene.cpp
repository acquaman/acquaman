/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMSignallingGraphicsScene.h"

#include <QGraphicsSceneMouseEvent>

AMSignallingGraphicsScene::AMSignallingGraphicsScene(QObject *parent) :
	QGraphicsScene(parent)
{
	doubleClickInProgress_ = false;
}

void AMSignallingGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	if(event->button() == Qt::LeftButton) {
		doubleClickInProgress_ = false;
	}
	QGraphicsScene::mousePressEvent(event);
}

#include <QApplication>
void AMSignallingGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

	if(event->button() == Qt::LeftButton && (event->buttonDownPos(Qt::LeftButton) - event->pos()).manhattanLength() < QApplication::startDragDistance()) {
		if(doubleClickInProgress_)
			emit doubleClicked(event->buttonDownScenePos(Qt::LeftButton));
		else
			emit clicked(event->buttonDownScenePos(Qt::LeftButton));
	}

	doubleClickInProgress_ = false;
	QGraphicsScene::mouseReleaseEvent(event);
}

void AMSignallingGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
	doubleClickInProgress_ = true;
	QGraphicsScene::mouseDoubleClickEvent(event);
}
