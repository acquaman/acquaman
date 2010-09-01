#ifndef AMSIGNALLINGGRAPHICSSCENE_H
#define AMSIGNALLINGGRAPHICSSCENE_H

#include <QGraphicsScene>

/// This class extends QGraphicsScene to deliver some useful events as signals
class AMSignallingGraphicsScene : public QGraphicsScene {
	Q_OBJECT
public:
	explicit AMSignallingGraphicsScene(QObject* parent = 0);

signals:
	/// Emitted when a point in the scene is clicked (on release). Location is in scene coordinates
	void clicked(QPointF pos);
	/// Emitted when a point in the scene is double clicked (on release). Location is in scene coordinates
	void doubleClicked(QPointF pos);

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);



protected:
	bool doubleClickInProgress_;
};

#endif // AMSIGNALLINGGRAPHICSSCENE_H
