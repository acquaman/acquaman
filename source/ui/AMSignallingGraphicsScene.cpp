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
