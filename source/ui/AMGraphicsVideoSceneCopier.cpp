#include "AMGraphicsVideoSceneCopier.h"

#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QObject>
#include <QTextDocument>
#include "GraphicsTextItem.h"
#include <QDebug>
#include <QTimer>

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
    /// need to figure out what items are new and put them into the old scene
    foreach(QGraphicsItem* item, oldList)
    {
        if(item->type() != QGraphicsItem::UserType)
        sceneToUpdate->removeItem(item);
    }

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
        if(listItem->type() == GraphicsTextItem::Type || listItem->type() == QGraphicsTextItem::Type)
        {
            QGraphicsTextItem* textItem = qgraphicsitem_cast<QGraphicsTextItem*>(listItem);
            if(!textItem)
            {
                textItem = qgraphicsitem_cast<GraphicsTextItem*>(listItem);
            }
            QGraphicsPolygonItem* polygonItem = qgraphicsitem_cast<QGraphicsPolygonItem*>(item);
            if(textItem && polygonItem && textItem->defaultTextColor() == polygonItem->pen().color())
            {
                  textItem->setPos(polygonItem->polygon().first().x(), polygonItem->polygon().first().y() - textItem->boundingRect().height());
            }
        }
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
    QGraphicsItem* newItem;
    int type = item->type();
    if(type == QGraphicsRectItem::Type)
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding rect";
        QGraphicsRectItem* rectItem = (QGraphicsRectItem*)item;
        QGraphicsRectItem* newRectItem = new QGraphicsRectItem();
        newRectItem->setZValue(rectItem->zValue());
        newRectItem->setPen(rectItem->pen());
        newRectItem->setRect(rectItem->rect());
        newItem = newRectItem;
    }
    else if(type == QGraphicsTextItem::Type)
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding text";
        QGraphicsTextItem* textItem = (QGraphicsTextItem*)item;
        QGraphicsTextItem* newTextItem = new QGraphicsTextItem();
        newTextItem->setZValue(0);
        newTextItem->setPlainText(textItem->document()->toPlainText());
        newTextItem->setFont(textItem->font());
        newTextItem->setDefaultTextColor(textItem->defaultTextColor());
        newItem = newTextItem;
    }
    else if(type == QGraphicsPolygonItem::Type)
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding poly";

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
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding line";
        QGraphicsLineItem* lineItem = (QGraphicsLineItem*)item;
        QGraphicsLineItem* newLineItem = new QGraphicsLineItem();
        newLineItem->setZValue(lineItem->zValue());
        newLineItem->setLine(lineItem->line());
        newLineItem->setPen(lineItem->pen());
        newItem = newLineItem;
    }
    else if(type == QGraphicsPathItem::Type)
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding path";
        QGraphicsPathItem* pathItem = (QGraphicsPathItem*)item;
        QGraphicsPathItem* newPathItem = new QGraphicsPathItem();
        newPathItem->setZValue(pathItem->zValue());
        newPathItem->setPath(pathItem->path());
        newPathItem->setPen(pathItem->pen());
        newItem = newPathItem;
    }
    else if(type == GraphicsTextItem::Type)
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding graphicsText";
        GraphicsTextItem* textItem = (GraphicsTextItem*)item;
        GraphicsTextItem* newTextItem = new GraphicsTextItem();
        newTextItem->setZValue(0);
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
        QGraphicsVideoItem* videoItem = dynamic_cast<QGraphicsVideoItem*>(item);
        if(videoItem != 0)
        {
            qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding video";
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
        {
//            qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding other custom";
             newItem = new QGraphicsPolygonItem();
        }
    }
    else
    {
//        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding other";
        newItem = new QGraphicsPolygonItem();
    }

    newItem->setX(item->x());
    newItem->setY(item->y());
    newItem->setVisible(item->isVisible());
    return newItem;
}
