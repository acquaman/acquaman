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


#include "AMOverlayVideoWidget2.h"

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QGraphicsVideoItem>
#endif

#include <QResizeEvent>
#include <QGLWidget>
#include <QLayout>
#include <QDebug>

AMOverlayVideoWidget2::AMOverlayVideoWidget2(QWidget *parent, bool useOpenGlViewport) :
	QGraphicsView(parent)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

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
	videoItem_->setAspectRatioMode(Qt::KeepAspectRatio);

	scene()->addItem(videoItem_);
	#endif
	resize(size());
}

AMOverlayVideoWidget2::~AMOverlayVideoWidget2() {
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

QSize AMOverlayVideoWidget2::sizeHint() const{
	return QSize(640, 512);
}

void AMOverlayVideoWidget2::resizeEvent(QResizeEvent *event)
{
	if (scene()) {
                scene()->setSceneRect(QRect(QPoint(0, 0), size()));
		#ifdef AM_MOBILITY_VIDEO_ENABLED
                videoItem_->setSize(size());
		#endif

        }
	QGraphicsView::resizeEvent(event);
}

