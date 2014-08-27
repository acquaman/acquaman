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


#include "AMOverlayVideoWidget.h"
#include <QResizeEvent>

#include <QGLWidget>

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QGraphicsVideoItem>
#endif


AMOverlayVideoWidget::AMOverlayVideoWidget(QWidget *parent, bool useOpenGlViewport) :
	QGraphicsView(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if(useOpenGlViewport) {
		setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}

	setScene(new QGraphicsScene());
	scene()->setBackgroundBrush(QBrush(QColor(50,50,50)));

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	videoItem_ = new QGraphicsVideoItem();
	mediaPlayer_ = new QMediaPlayer();
	mediaPlayer_->setVideoOutput(videoItem_);
	#endif

	scene()->addItem(videoItem_);
	resize(size());
}

AMOverlayVideoWidget::~AMOverlayVideoWidget() {
	// necessary to avoid a bug on Mac, where if the video widget is deleted while the mediaPlayer is still playing, and then the media player is deleted (also while still playing), there is a crash in:
	/* QT7MovieViewRenderer::setupVideoOutput() from
  QT7PlayerSession::~Qt7PlayerSession() from
  QMediaService::~QMediaService()
  QMediaPlayer::~QMediaPlayer()
  */
	#ifdef AM_MOBILITY_VIDEO_ENABLED
	mediaPlayer_->setMedia(QMediaContent());

	delete videoItem_;
	delete mediaPlayer_;
	#endif
}

void AMOverlayVideoWidget::resizeEvent(QResizeEvent *event)
{
	if (scene()) {
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
		videoItem_->setSize(event->size());
	}
	QGraphicsView::resizeEvent(event);
}

