/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMGraphicsVideoSceneCopier.h"

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#endif

#include <QGraphicsScene>
#include <QObject>
#include <QTextDocument>
#include <QDebug>
#include <QTimer>
#include <QWidget>

#include "ui/AMGraphicsTextItem.h"
#include "util/AMErrorMonitor.h"

AMGraphicsVideoSceneCopier::AMGraphicsVideoSceneCopier(QWidget* parent)
	: QObject(parent)
{
}

QGraphicsScene *AMGraphicsVideoSceneCopier::originalScene()
{
	return originalScene_;
}

QGraphicsScene *AMGraphicsVideoSceneCopier::scene()
{
	return scene_;
}

void AMGraphicsVideoSceneCopier::updateScene(QGraphicsScene* sceneToUpdate,QGraphicsScene* sceneToUpdateWith)
{
	QList<QGraphicsItem*> oldList = sceneToUpdate->items();
	QList<QGraphicsItem*> newList = sceneToUpdateWith->items();
	QGraphicsItem* videoItem;
	while(!oldList.isEmpty())
	{
		QGraphicsItem* item = oldList.takeLast();
		sceneToUpdate->removeItem(item);
		if(item->type() == QGraphicsItem::UserType)
		{
			videoItem = item;
		}


	}

	sceneToUpdate->addItem(videoItem);
	foreach(QGraphicsItem* item, newList)
	{

		if(item->type() != QGraphicsItem::UserType)
		{

			QGraphicsItem* newItem = getCopy(item);
			sceneToUpdate->addItem(newItem);
		}
	}


}

void AMGraphicsVideoSceneCopier::updateShape(QGraphicsItem *item, QGraphicsScene* scene)
{
	QList<QGraphicsItem*> list = scene->items();
	foreach(QGraphicsItem* listItem, list)
	{
		if(listItem->type() == item->type())
		{
			if(item->type() == QGraphicsPolygonItem::Type)
			{
				QGraphicsPolygonItem* newPolygonItem =  qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
				QGraphicsPolygonItem* listPolygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(listItem);
				if(newPolygonItem && listPolygonItem)
				{
					if(listPolygonItem->pen() == newPolygonItem->pen())
					{
						listPolygonItem->setPolygon(newPolygonItem->polygon());
						listItem = listPolygonItem;
					}
				}
			}


		}
		if(listItem->type() == AMGraphicsTextItem::Type || listItem->type() == QGraphicsTextItem::Type)
		{
			QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(listItem);
			if(!textItem)
			{
				textItem = qgraphicsitem_cast<AMGraphicsTextItem*>(listItem);
			}
			QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
			if(textItem && polygonItem && textItem->defaultTextColor() == polygonItem->pen().color())
			{
				textItem->setPos(polygonItem->polygon().first().x(), polygonItem->polygon().first().y() - textItem->boundingRect().height());
			}
		}
	}
}

void AMGraphicsVideoSceneCopier::updateChange(QGraphicsScene *sceneToUpdate, QGraphicsScene *sceneToUpdateWith)
{
	QList<QGraphicsItem*> oldList = sceneToUpdate->items();
	QList<QGraphicsItem*> newList = sceneToUpdateWith->items();

	sceneToUpdate->setSceneRect(sceneToUpdateWith->sceneRect());
//	AMErrorMon::debug(this, AMGRAPHICSVIDEOSCENECOPIER_DEBUG_OUTPUT, QString("Scene rect: Old: %1 %2 New: %3 %4").arg(sceneToUpdate->sceneRect().x()).arg(sceneToUpdate->sceneRect().y()).arg(sceneToUpdateWith->sceneRect().y()).arg(sceneToUpdateWith->sceneRect().y()) );

	foreach(QGraphicsItem* oldItems, sceneToUpdate->items())
	{
		if(oldItems->type() == QGraphicsItem::UserType)
		{
			#ifdef AM_MOBILITY_VIDEO_ENABLED
			QGraphicsVideoItem * videoItem = qgraphicsitem_cast<QGraphicsVideoItem*>(oldItems);
			if(videoItem)
			{
				sceneToUpdate->removeItem(videoItem);
				videoItem->setX(0);
				videoItem->setY(0);
				sceneToUpdate->addItem(videoItem);
				videoItem->setSize(sceneToUpdate->sceneRect().size());
			}
			#endif
		}
	}


	/// if the lists are not the same length, have to do a full update
	if(oldList.count() != newList.count())
	{
		updateScene(sceneToUpdate,sceneToUpdateWith);
		return;
	}

	/// create sublists that ignore the graphics item.
	/// the oldList is in reverse order of the newList
	QList<QGraphicsItem*> oldItems;
	QList<QGraphicsItem*> newItems;
	QList<QList<QGraphicsItem*> > oldListList;
	QList<int> zValues;

	foreach(QGraphicsItem* newItem, newList)
	{
		if(newItem->type() != QGraphicsItem::UserType)
		{
			newItems.append(newItem);
		}
	}
	QGraphicsItem* oldItem;
	for(int i = 0; i < oldList.count(); i++)
	{

		oldItem = oldList[(oldList.count() - 1) - i];
		if(oldItem->type() != QGraphicsItem::UserType)
		{
			int currentZ = oldItem->zValue();
			if(zValues.contains(currentZ))
			{
				oldListList[zValues.indexOf(currentZ)].append(oldItem);
			}
			else
			{
				QList<QGraphicsItem*> subList;
				subList.append(oldItem);
				zValues.append(currentZ);
				oldListList.append(subList);
			}
		}
	}
	for(int i = 0; i < oldListList.count(); i++)
	{
		oldItems<<oldListList[oldListList.count() -1 -i];
	}

	/// should now have two lists of equivalent shapes.
	/// must go through each item in oldItems, check for equivalence
	/// with the corresponding newItems, and update them if necessary
	/// just leave the video item be
	bool success = true;
	for(int i = 0; i < oldItems.count(); i++)
	{
		success = updateItem(oldItems[i], newItems[i]);
		if(!success)
		{
			updateScene(sceneToUpdate, sceneToUpdateWith);
			return;
		}
	}
}

bool AMGraphicsVideoSceneCopier::updateItem(QGraphicsItem *itemToUpdate, QGraphicsItem *itemToCopy)
{
	if(itemToUpdate->type() != itemToCopy->type())
	{
		AMErrorMon::debug(this, AMGRAPHICSVIDEOSCENECOPIER_DEBUG_OUTPUT, QString("Call to updateItem. Update type: %1 Copy type: %2").arg(itemToUpdate->type()).arg(itemToCopy->type()) );
		bool textTypes;
		textTypes = (itemToUpdate->type() == QGraphicsTextItem::Type && itemToCopy->type() == AMGraphicsTextItem::Type) || (itemToUpdate->type() == AMGraphicsTextItem::Type && itemToCopy->type() == QGraphicsTextItem::Type);
		if(!textTypes)
			return false;
		return true;
	}
	bool equal = getEquivalent(itemToUpdate,itemToCopy);
	if(!equal)
	{
		copyItem(itemToUpdate, itemToCopy);
	}
	return true;

}

bool AMGraphicsVideoSceneCopier::getEquivalent(QGraphicsItem *itemOne, QGraphicsItem *itemTwo)
{
	if(itemOne->type() != itemTwo->type())
		return false;
	else if(itemOne->pos() != itemTwo->pos())
		return false;
	else if(itemOne->zValue() != itemTwo->zValue())
		return false;

	QGraphicsRectItem* rectOne;
	QGraphicsRectItem* rectTwo;
	QGraphicsPolygonItem* polyOne;
	QGraphicsPolygonItem* polyTwo;
	QGraphicsLineItem* lineOne;
	QGraphicsLineItem* lineTwo;
	QGraphicsPathItem* pathOne;
	QGraphicsPathItem* pathTwo;
	QGraphicsTextItem* textOne;
	QGraphicsTextItem* textTwo;

	switch(itemOne->type())
	{
	case QGraphicsRectItem::Type:
		rectOne = qgraphicsitem_cast<QGraphicsRectItem*>(itemOne);
		rectTwo = qgraphicsitem_cast<QGraphicsRectItem*>(itemTwo);
		if(!rectOne || !rectTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_RECT, QString("Failed to cast to rect.") );
			return false;
		}
		if(rectOne->rect() != rectTwo->rect())
			return false;
		if(rectOne->pen() != rectTwo->pen())
			return false;
		if(rectOne->brush() != rectTwo->brush())
			return false;
		return true;
		break;
	case QGraphicsPolygonItem::Type:
		polyOne = qgraphicsitem_cast<QGraphicsPolygonItem*>(itemOne);
		polyTwo = qgraphicsitem_cast<QGraphicsPolygonItem*>(itemTwo);
		if(!polyOne || !polyTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_POLYGON, QString("Failed to cast to polygon.") );
			return false;
		}
		if(polyOne->polygon() != polyTwo->polygon())
			return false;
		else if(polyOne->pen() != polyTwo->pen())
			return false;
		else if(polyOne->brush() != polyTwo->brush())
			return false;
		else
			return true;
		break;
	case QGraphicsLineItem::Type:
		lineOne = qgraphicsitem_cast<QGraphicsLineItem*>(itemOne);
		lineTwo = qgraphicsitem_cast<QGraphicsLineItem*>(itemTwo);
		if(!lineOne || !lineTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_LINE, QString("Failed to cast to line.") );
			return false;
		}
		if(lineOne->line() != lineTwo->line())
			return false;
		else if(lineOne->pen() != lineTwo->pen())
			return false;
		else
			return true;
		break;
	case QGraphicsPathItem::Type:
		pathOne = qgraphicsitem_cast<QGraphicsPathItem*>(itemOne);
		pathTwo = qgraphicsitem_cast<QGraphicsPathItem*>(itemTwo);
		if(!pathOne || !pathTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_PATH, QString("Failed to cast to path.") );
			return false;
		}
		if(pathOne->path() != pathTwo->path())
			return false;
		else if(pathOne->pen() != pathTwo->pen())
			return false;
		else if(pathOne->brush() != pathTwo->brush())
			return false;
		else
			return true;
		return true;
		break;
	case QGraphicsTextItem::Type:
	case AMGraphicsTextItem::Type:
		textOne = (QGraphicsTextItem*)(itemOne);
		textTwo = (QGraphicsTextItem*)(itemTwo);
		if(!textOne || !textTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_TEXT, QString("Failed to cast to text.") );
			return false;
		}
		if(textOne->document()->toPlainText() != textTwo->document()->toPlainText())
			return false;
		else if(textOne->defaultTextColor() != textTwo->defaultTextColor())
			return false;
		else
			return true;
	default:
		AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_GETEQUIVALENT_CAST_FAILED_UNKNOWN_TYPE, QString("Failed to cast to unknown type %1 %2.").arg(itemOne->type()).arg(itemTwo->type()) );
		return false;
	}
}

void AMGraphicsVideoSceneCopier::setOriginalScene(QGraphicsScene *originalScene)
{
	originalScene_ = originalScene;
	cloneScene();
}

void AMGraphicsVideoSceneCopier::cloneScene()
{
	QGraphicsScene* oldScene;
	oldScene = originalScene_;
	QRectF sceneRect = oldScene->sceneRect();
	QGraphicsScene* newScene = new QGraphicsScene();
	newScene->setSceneRect(sceneRect.x(),sceneRect.y(),sceneRect.width(),sceneRect.height());

	QList<QGraphicsItem*> sceneList = oldScene->items();



	foreach(QGraphicsItem* item, sceneList)
	{
		QGraphicsItem* newItem = getCopy(item);

		newScene->addItem(newItem);
	}
	newScene->setBackgroundBrush(oldScene->backgroundBrush());
	scene_ = newScene;
	emit sceneCloned();
}

void AMGraphicsVideoSceneCopier::cloneScene(QGraphicsScene *originalScene)
{
	setOriginalScene(originalScene);
}


QGraphicsItem *AMGraphicsVideoSceneCopier::getCopy(QGraphicsItem *item)
{
    QGraphicsItem* newItem = 0;
	int type = item->type();
	if(type == QGraphicsRectItem::Type)
	{
		QGraphicsRectItem* rectItem = (QGraphicsRectItem*)item;
		QGraphicsRectItem* newRectItem = new QGraphicsRectItem();
		newRectItem->setZValue(rectItem->zValue());
		newRectItem->setPen(rectItem->pen());
		newRectItem->setRect(rectItem->rect());
		newItem = newRectItem;
	}
	else if(type == QGraphicsTextItem::Type)
	{
		QGraphicsTextItem* textItem = (QGraphicsTextItem*)item;
		QGraphicsTextItem* newTextItem = new QGraphicsTextItem();
		newTextItem->setZValue(textItem->zValue());
		newTextItem->setPlainText(textItem->document()->toPlainText());
		newTextItem->setFont(textItem->font());
		newTextItem->setDefaultTextColor(textItem->defaultTextColor());
		newItem = newTextItem;
	}
	else if(type == QGraphicsPolygonItem::Type)
	{

		QGraphicsPolygonItem* polyItem = (QGraphicsPolygonItem*)item;
		QGraphicsPolygonItem* newPolyItem = new QGraphicsPolygonItem();
		newPolyItem->setZValue(polyItem->zValue());
		newPolyItem->setPen(polyItem->pen());
		newPolyItem->setBrush(polyItem->brush());
		newPolyItem->setPolygon(polyItem->polygon());
		newItem = newPolyItem;

	}
	else if(type == QGraphicsLineItem::Type)
	{
		QGraphicsLineItem* lineItem = (QGraphicsLineItem*)item;
		QGraphicsLineItem* newLineItem = new QGraphicsLineItem();
		newLineItem->setZValue(lineItem->zValue());
		newLineItem->setLine(lineItem->line());
		newLineItem->setPen(lineItem->pen());
		newItem = newLineItem;
	}
	else if(type == QGraphicsPathItem::Type)
	{
		QGraphicsPathItem* pathItem = (QGraphicsPathItem*)item;
		QGraphicsPathItem* newPathItem = new QGraphicsPathItem();
		newPathItem->setZValue(pathItem->zValue());
		newPathItem->setPath(pathItem->path());
		newPathItem->setPen(pathItem->pen());
		newItem = newPathItem;
	}
	else if(type == AMGraphicsTextItem::Type)
	{
		AMGraphicsTextItem* textItem = (AMGraphicsTextItem*)item;
		AMGraphicsTextItem* newTextItem = new AMGraphicsTextItem();
		newTextItem->setZValue(textItem->zValue());
		newTextItem->setPlainText(textItem->document()->toPlainText());
		newTextItem->setFont(textItem->font());
		newTextItem->setDefaultTextColor(textItem->defaultTextColor());
		newItem = newTextItem;
	}
	/// QGraphics video item has type 1 (equal to QGraphics item)
	/// however when it is taken from the list the item has type
	/// QGraphicsItem::UserType
	/// Since this could be used for subclasses, have to check for
	/// correct type when casting
	else if(type == QGraphicsItem::UserType)
	{
#ifdef AM_MOBILITY_VIDEO_ENABLED
		QGraphicsVideoItem* videoItem = dynamic_cast<QGraphicsVideoItem*>(item);
		if(videoItem != 0)
		{

			QGraphicsVideoItem* newVideoItem = new QGraphicsVideoItem();
			newVideoItem->setZValue(-1);
			QMediaPlayer* mediaPlayer = new QMediaPlayer();
			QMediaPlayer* oldMediaPlayer = qobject_cast<QMediaPlayer*>(videoItem->mediaObject());
			QMediaContent media = QUrl();
			if(oldMediaPlayer != 0)
				media = oldMediaPlayer->media();
			mediaPlayer->setMedia(media);
			mediaPlayer->setVideoOutput(newVideoItem);
			mediaPlayer->play();
			newVideoItem->setPos(item->pos());
			newVideoItem->setSize(videoItem->size());
			newItem = newVideoItem;
		}
		else
			newItem = new QGraphicsPolygonItem();
#endif
	}
	else
		newItem = new QGraphicsPolygonItem();

	newItem->setX(item->x());
	newItem->setY(item->y());
	newItem->setVisible(item->isVisible());
	return newItem;
}

void AMGraphicsVideoSceneCopier::copyItem(QGraphicsItem *itemToUpdate, QGraphicsItem *itemToCopy)
{
	QGraphicsRectItem* rectOne;
	QGraphicsRectItem* rectTwo;
	QGraphicsPolygonItem* polyOne;
	QGraphicsPolygonItem* polyTwo;
	QGraphicsLineItem* lineOne;
	QGraphicsLineItem* lineTwo;
	QGraphicsPathItem* pathOne;
	QGraphicsPathItem* pathTwo;
	QGraphicsTextItem* textOne;
	QGraphicsTextItem* textTwo;


	itemToUpdate->setPos(itemToCopy->pos());
	itemToUpdate->setZValue(itemToCopy->zValue());

	switch(itemToUpdate->type())
	{
	case QGraphicsRectItem::Type:
		rectOne = qgraphicsitem_cast<QGraphicsRectItem*>(itemToUpdate);
		rectTwo = qgraphicsitem_cast<QGraphicsRectItem*>(itemToCopy);
		if(!rectOne || !rectTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_RECT, QString("Failed to cast to rect.") );
			return;
		}
		rectOne->setRect(rectTwo->rect());
		rectOne->setPen(rectTwo->pen());
		rectOne->setBrush(rectTwo->brush());
		return;
	case QGraphicsPolygonItem::Type:
		polyOne = qgraphicsitem_cast<QGraphicsPolygonItem*>(itemToUpdate);
		polyTwo = qgraphicsitem_cast<QGraphicsPolygonItem*>(itemToCopy);
		if(!polyOne || !polyTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_POLYGON, QString("Failed to cast to polygon.") );
			return;
		}
		polyOne->setPolygon(polyTwo->polygon());
		polyOne->setPen(polyTwo->pen());
		polyOne->setBrush(polyTwo->brush());
		return;
	case QGraphicsLineItem::Type:
		lineOne = qgraphicsitem_cast<QGraphicsLineItem*>(itemToUpdate);
		lineTwo = qgraphicsitem_cast<QGraphicsLineItem*>(itemToCopy);
		if(!lineOne || !lineTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_LINE, QString("Failed to cast to line.") );
			return;
		}
		lineOne->setLine(lineTwo->line());
		lineOne->setPen(lineTwo->pen());
		return;
	case QGraphicsPathItem::Type:
		pathOne = qgraphicsitem_cast<QGraphicsPathItem*>(itemToUpdate);
		pathTwo = qgraphicsitem_cast<QGraphicsPathItem*>(itemToCopy);
		if(!pathOne || !pathTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_PATH, QString("Failed to cast to path.") );
			return;
		}
		pathOne->setPath(pathTwo->path());
		pathOne->setPen(pathTwo->pen());
		pathOne->setBrush(pathTwo->brush());
		return;
	case QGraphicsTextItem::Type:
	case AMGraphicsTextItem::Type:
		textOne = (QGraphicsTextItem*)(itemToUpdate);
		textTwo = (QGraphicsTextItem*)(itemToCopy);
		if(!textOne || !textTwo)
		{
			AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_TEXT, QString("Failed to cast to text.") );
			return;
		}
		textOne->setPlainText(textTwo->document()->toPlainText());
		textOne->setDefaultTextColor(textTwo->defaultTextColor());
		return;
	default:
		AMErrorMon::alert(this, AMGRAPHICSVIDEOSCENECOPIER_COPYITEM_CAST_FAILED_UNKNOWN_TYPE, QString("Failed to cast to unknown type %1.").arg(itemToUpdate->type()) );
		return;
	}
}


