/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
