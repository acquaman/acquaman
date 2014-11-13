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


#ifndef AMOVERLAYVIDEOWIDGET2_H
#define AMOVERLAYVIDEOWIDGET2_H

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QMediaPlayer>
#include <QGraphicsVideoItem>
#endif

#include <QGraphicsView>

#ifdef AM_MOBILITY_VIDEO_ENABLED
class AMGraphicsVideoItem : public QGraphicsVideoItem
{
Q_OBJECT

public:
	AMGraphicsVideoItem(QGraphicsItem *parent = 0);
	virtual ~AMGraphicsVideoItem();

	bool paintBlack() const;

	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public slots:
	void setPaintBlack(bool paintBlack);

protected:
	bool paintBlack_;
};
#endif

/// Ever wanted to draw on top of live video? This is a convenience class that provides a QMediaPlayer, and a QGraphicsVideoItem embedded  inside a QGraphicsScene, so that other QGraphicsItems can be painted on top of it.
/*! The class itself is a QGraphicsView widget, and it creates the following objects automatically for you:

  - a QGraphicsScene to display all the visual stuff. (Access this via scene() to add more items.)
  - a QGraphicsVideoItem that actually renders the video.
  - a QMediaPlayer to load and control the video. Access this (for example, to set the URL of the video to play using setMedia()) via mediaPlayer().

  <b>Resizing</b>

  When this widget is resized, the scene inside the view it will be resized to keep the same size as the whole widget.  The internal video item will also be resized to fill the entire scene.
  */
class AMOverlayVideoWidget2 : public QGraphicsView
{
	Q_OBJECT
public:
	/// Constructor.  If you don't want to (or can't) use OpenGL acceleration on the view, you can set \c useOpenGLView to false. However, leaving it on provides a huge reduction in CPU usage, at least on Mac OS X.
	explicit AMOverlayVideoWidget2(QWidget *parent = 0, bool useOpenGlView = true);
	/// Destructor.
	~AMOverlayVideoWidget2();

	/// Access the internal QMediaPlayer that controls playback of video.
	/*! For example, to use this widget to start playing a network stream:

	  \code
	  mediaPlayer()->setMedia(QUrl("http://v2e1607-001.cs.clsi.ca/mjpg/1/video.mjpg"));
	  mediaPlayer()->play();
	  \endcode

	  \note QMediaPlayer is part of the QMultimediaKit module in the QtMobility add-on component. It may be included in Qt proper in 4.8 or 5.  See http://doc.qt.nokia.com/qtmobility-1.1.3/multimedia.html for details.
	  */
	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QMediaPlayer* mediaPlayer() { return mediaPlayer_; }

	/// Access the QGraphicsVideoItem embedded inside this QGraphicsView/scene.
	/*! \note QGraphicsVideoItem is part of the QMultimediaKit module in the QtMobility add-on component. It may be included in Qt proper in 4.8 or 5.  See http://doc.qt.nokia.com/qtmobility-1.1.3/multimedia.html for details.
	  */
	QGraphicsVideoItem* videoItem() { return videoItem_; }
	#endif

	void setPaintBlack(bool paintBlack);

	virtual QSize sizeHint() const;

signals:

public slots:

protected:
	/// When this widget is resized, the scene inside the view it will be resized to keep the same size as the whole widget.  The internal videoItem() will also be resized to fill the entire scene.
	void resizeEvent(QResizeEvent *event);

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	AMGraphicsVideoItem* videoItem_;
	QMediaPlayer* mediaPlayer_;
	#endif
};

#endif // AMOVERLAYVIDEOWIDGET_H
