#include "AMGraphicsVideoSceneCopier.h"

#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QObject>
#include <QTextDocument>
#include "GraphicsTextItem.h"
#include <QDebug>

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
    qDebug()<<"AMGraphicsVideoSceneCopier::updateScene - updating scene";
    QList<QGraphicsItem*> oldList = sceneToUpdate->items();
    QList<QGraphicsItem*> newList = sceneToUpdateWith->items();
    /// need to figure out what items are new and put them into the old scene
    // add new items
    int newCount = newList.count();
    int oldCount = oldList.count();
    qDebug()<<"AMGraphicsVideoSceneCopier::updateScene"<<newCount<<oldCount;
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

    QGraphicsTextItem* textFixItem = new QGraphicsTextItem("Fix");
    textFixItem->setObjectName("Fix");
    textFixItem->setZValue(INT_MAX);
    textFixItem->setPos(-1*textFixItem->boundingRect().width(), -1*textFixItem->boundingRect().height());
    newScene->addItem(textFixItem);

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
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding rect";
        QGraphicsRectItem* rectItem = (QGraphicsRectItem*)item;
        QGraphicsRectItem* newRectItem = new QGraphicsRectItem();
        newRectItem->setZValue(rectItem->zValue());
        newRectItem->setPen(rectItem->pen());
        newRectItem->setRect(rectItem->rect());
        newItem = newRectItem;
    }
    else if(type == QGraphicsTextItem::Type)
    {
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding text";
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
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding poly";

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
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding line";
        QGraphicsLineItem* lineItem = (QGraphicsLineItem*)item;
        QGraphicsLineItem* newLineItem = new QGraphicsLineItem();
        newLineItem->setZValue(lineItem->zValue());
        newLineItem->setLine(lineItem->line());
        newLineItem->setPen(lineItem->pen());
        newItem = newLineItem;
    }
    else if(type == QGraphicsPathItem::Type)
    {
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding path";
        QGraphicsPathItem* pathItem = (QGraphicsPathItem*)item;
        QGraphicsPathItem* newPathItem = new QGraphicsPathItem();
        newPathItem->setZValue(pathItem->zValue());
        newPathItem->setPath(pathItem->path());
        newPathItem->setPen(pathItem->pen());
        newItem = newPathItem;
    }
    else if(type == GraphicsTextItem::Type)
    {
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding graphicsText";
        GraphicsTextItem* textItem = (GraphicsTextItem*)item;
        GraphicsTextItem* newTextItem = new GraphicsTextItem();
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
            qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding other custom";
             newItem = new QGraphicsPolygonItem();
        }
    }
    else
    {
        qDebug()<<"AMGraphicsVideoSceneCopier::getCopy - adding other";
        newItem = new QGraphicsPolygonItem();
    }

    newItem->setX(item->x());
    newItem->setY(item->y());
    newItem->setVisible(item->isVisible());
    return newItem;
}
