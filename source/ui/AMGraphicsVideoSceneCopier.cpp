#include "AMGraphicsVideoSceneCopier.h"

#include <QGraphicsScene>
#include <QGraphicsVideoItem>
#include <QMediaPlayer>
#include <QObject>
#include <QTextDocument>

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
    QGraphicsScene* newScene = new QGraphicsScene(sceneRect);

    QList<QGraphicsItem*> sceneList = oldScene->items();

    foreach(QGraphicsItem* item, sceneList)
    {
        QGraphicsItem* newItem;
        int type = item->type();
        if(type == QGraphicsRectItem::Type)
        {
            QGraphicsRectItem* rectItem = (QGraphicsRectItem*)item;
            QGraphicsRectItem* newRectItem = new QGraphicsRectItem();
            newRectItem->setX(rectItem->x());
            newRectItem->setY(rectItem->y());
            newRectItem->setZValue(rectItem->zValue());
            newRectItem->setPen(rectItem->pen());
            newRectItem->setRect(rectItem->rect());
            newItem = newRectItem;
        }
        else if(type == QGraphicsTextItem::Type)
        {
            QGraphicsTextItem* textItem = (QGraphicsTextItem*)item;
            QGraphicsTextItem* newTextItem = new QGraphicsTextItem();
            newTextItem->setX(textItem->x());
            newTextItem->setY(textItem->y());
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
            newPolyItem->setX(polyItem->x());
            newPolyItem->setY(polyItem->y());
            newPolyItem->setZValue(polyItem->zValue());
            newPolyItem->setPen(polyItem->pen());
            newPolyItem->setBrush(polyItem->brush());
            newPolyItem->setPolygon(polyItem->polygon());
            newItem = newPolyItem;

        }
        else if(type == QGraphicsVideoItem::Type)
        {
            QMediaPlayer* mediaPlayer = new QMediaPlayer();
            QGraphicsVideoItem* videoItem = (QGraphicsVideoItem*)item;
            QGraphicsVideoItem* newVideoItem = new QGraphicsVideoItem;
            newVideoItem->setX(videoItem->x());
            newVideoItem->setY(videoItem->y());
            newVideoItem->setZValue(videoItem->zValue());
            mediaPlayer->setMedia(QUrl("http://10.52.48.104/axis-cgi/mjpg/video.cgi?resolution=CIF"));
            mediaPlayer->setVideoOutput(newVideoItem);
            mediaPlayer->play();
            newItem = newVideoItem;
        }
        else if(type == QGraphicsLineItem::Type)
        {
            QGraphicsLineItem* lineItem = (QGraphicsLineItem*)item;
            QGraphicsLineItem* newLineItem = new QGraphicsLineItem();
            newLineItem->setX(lineItem->x());
            newLineItem->setY(lineItem->y());
            newLineItem->setZValue(lineItem->zValue());
            newLineItem->setLine(lineItem->line());
            newLineItem->setPen(lineItem->pen());
            newItem = newLineItem;
        }
        else if(type == QGraphicsPathItem::Type)
        {
            QGraphicsPathItem* pathItem = (QGraphicsPathItem*)item;
            QGraphicsPathItem* newPathItem = new QGraphicsPathItem();
            newPathItem->setX(pathItem->x());
            newPathItem->setY(pathItem->y());
            newPathItem->setZValue(pathItem->zValue());
            newPathItem->setPath(pathItem->path());
            newPathItem->setPen(pathItem->pen());
            newItem = newPathItem;
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
                QGraphicsVideoItem* newVideoItem = new QGraphicsVideoItem();
                newVideoItem->setX(item->x());
                newVideoItem->setY(item->y());
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
                 newItem = new QGraphicsPolygonItem();
            }
        }
        else
        {
            newItem = new QGraphicsPolygonItem();
        }
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
