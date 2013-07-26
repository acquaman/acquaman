#ifndef AMGRAPHICSVIDEOSCENECOPIER_H
#define AMGRAPHICSVIDEOSCENECOPIER_H

#include <QObject>

class QGraphicsScene;
class QObject;
class QGraphicsItem;


/// This class copies a scene for use in another view
/// can be used to duplicate a scene with a video widget
class AMGraphicsVideoSceneCopier : public QObject
{
    Q_OBJECT
public:
    /// constructor
    AMGraphicsVideoSceneCopier(QWidget* parent = 0);

    /// the scene that was copied
    QGraphicsScene* originalScene();

    /// the copy
    QGraphicsScene* scene();

    /// updates the whole scene
    void updateScene(QGraphicsScene* sceneToUpdate,QGraphicsScene* sceneToUpdateWith);


    /// updates a single QGraphicsItem
    void updateShape(QGraphicsItem* item, QGraphicsScene* sceneToUpdate);

    /// attempts to update only the shapes that it needs to
    /// if the number of items in either scene has changed it has
    /// to do a full updateScene
    void updateChange(QGraphicsScene* sceneToUpdate, QGraphicsScene* sceneToUpdateWith);

    bool updateItem(QGraphicsItem* itemToUpdate, QGraphicsItem* itemToCopy);

    bool getEquivalent(QGraphicsItem* itemOne, QGraphicsItem* itemTwo);



public slots:
    /// sets the original scene and copies it
    void setOriginalScene(QGraphicsScene* originalScene);
    /// creates a copy of the current original scene
    void cloneScene();
    /// sets the original scene and copies it (exactly the same as calling setOriginalScene)
    void cloneScene(QGraphicsScene* originalScene);

signals:
    /// emitted after the scene has been copied
    void sceneCloned();

protected:
    QGraphicsItem* getCopy(QGraphicsItem* item);
    void copyItem(QGraphicsItem* itemToUpdate, QGraphicsItem* itemToCopy);

protected:

    QGraphicsScene* originalScene_;
    QGraphicsScene* scene_;

};

#endif // AMGRAPHICSVIDEOSCENECOPIER_H
