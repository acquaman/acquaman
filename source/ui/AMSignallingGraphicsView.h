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


#ifndef AMSIGNALLINGGRAPHICSSCENE_H
#define AMSIGNALLINGGRAPHICSSCENE_H

#include <QGraphicsView>
#include <QTimer>

/// This class extends QGraphicsScene to deliver some useful events as signals
class AMSignallingGraphicsView : public QGraphicsView {
	Q_OBJECT
public:
 	virtual ~AMSignallingGraphicsView();
	explicit AMSignallingGraphicsView(QWidget* parent = 0);

signals:
	/// Emitted when a point in the scene is clicked (on release). Location is in view coordinates. This could be the first release of a double-click.
	void clicked(const QPoint& pos);
	/// Emitted when a point in the scene is clicked, and enough time has passed since release to know that it's not a double-click. Location is in view coordinates.
	void singleClicked(const QPoint& pos);
	/// Emitted when a point in the scene is double clicked (on release). Location is in view coordinates
	void doubleClicked(const QPoint& pos);

	/// Emitted when a drag is started. Location is the start of the drag, in view coordinates.
	void dragStarted(const QPoint& startPos, const QPoint& currentPos);
	/// Emitted while a drag is being moved. Locations are the start and current position of the drag, in view coordintes.
	void dragMoved(const QPoint& startPos, const QPoint& currentPos);
	/// Emitted when a drag is finished (ie: left mouse button is released)
	void dragEnded(const QPoint& startPos, const QPoint& endPos);

protected:
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

protected slots:
	void onSingleClickTimerTimeout();

protected:
	bool doubleClickInProgress_;
	QPoint buttonDownPos_;
	bool dragStartInProgress_;
	bool dragInProgress_;
	QTimer singleClickTimer_;
};

#endif // AMSIGNALLINGGRAPHICSSCENE_H
