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


#ifndef AMSHAPEDATASETGRAPHICSVIEW_H
#define AMSHAPEDATASETGRAPHICSVIEW_H

#include "ui/AMOverlayVideoWidget2.h"

/**
 * @brief The AMSampleCameraGraphicsView class is a view for showing
 *	the sampleCameraView contents.
 */
class AMSampleCameraGraphicsView : public AMOverlayVideoWidget2
{
	Q_OBJECT
public:
	explicit AMSampleCameraGraphicsView(QWidget *parent = 0, bool useOpenGlViewport = true);

	void resizeEvent(QResizeEvent *event);

	void mousePressEvent(QMouseEvent *event);

	void mouseReleaseEvent(QMouseEvent *event);

	void mouseDoubleClickEvent(QMouseEvent *event);

	void mouseMoveEvent(QMouseEvent *event);

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	void setVideoItem(QGraphicsVideoItem* item);
	#endif

	/// Helper function to convert scene coordinates to video-relative coordinates. (This is only tricky because depending on the videoItem()'s aspectRatioMode(), there might be letterbox bars at the top or sides of the display.
	QPointF mapSceneToVideo(const QPointF& sceneCoordinate) const;
	/// Inverse of mapSceneToVideo, takes a relative point and gives it a screen position.
	QPointF mapVideoToScene(const QPointF &videoCoordinate) const;

signals:
	void resize();

	void mousePressed(const QPointF& position);

	void mouseRightClicked(const QPointF& position);

	void mouseLeftReleased(const QPointF& position);

	void mouseRightReleased(const QPointF& position);

	void mouseDoubleClicked(const QPointF& position);

	void mouseMoved(const QPointF& position);

protected:
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif // AMSHAPEDATASETGRAPHICSVIEW_H
