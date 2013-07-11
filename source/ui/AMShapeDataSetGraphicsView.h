#ifndef AMSHAPEDATASETGRAPHICSVIEW_H
#define AMSHAPEDATASETGRAPHICSVIEW_H

#include "AMOverlayVideoWidget2.h"


class AMShapeDataSetGraphicsView : public AMOverlayVideoWidget2
{
    Q_OBJECT
public:
    explicit AMShapeDataSetGraphicsView(QWidget *parent, bool useOpenGlViewport = true);

    void resizeEvent(QResizeEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

signals:

    void resize();

    void mousePressed(const QPointF& position);

    void mouseRightClicked(const QPointF& position);

    void mouseLeftReleased(const QPointF& position);

    void mouseRightReleased(const QPointF& position);

    void mouseDoubleClicked(const QPointF& position);

    void mouseMoved(const QPointF& position);

protected:

    /// Helper function to convert scene coordinates to video-relative coordinates. (This is only tricky because depending on the videoItem()'s aspectRatioMode(), there might be letterbox bars at the top or sides of the display.
    QPointF mapSceneToVideo(const QPointF& sceneCoordinate) const;

    void contextMenuEvent(QContextMenuEvent *event);





};

#endif // AMSHAPEDATASETGRAPHICSVIEW_H
