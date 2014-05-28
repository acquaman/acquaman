#ifndef AMOVERLAYVIDEOWIDGET2_H
#define AMOVERLAYVIDEOWIDGET2_H

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include <QMediaPlayer>
class QGraphicsVideoItem;
#endif

#include <QGraphicsView>


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

	virtual QSize sizeHint() const;

signals:

public slots:

protected:
	/// When this widget is resized, the scene inside the view it will be resized to keep the same size as the whole widget.  The internal videoItem() will also be resized to fill the entire scene.
	void resizeEvent(QResizeEvent *event);

	#ifdef AM_MOBILITY_VIDEO_ENABLED
	QGraphicsVideoItem* videoItem_;
	QMediaPlayer* mediaPlayer_;
	#endif
};

#endif // AMOVERLAYVIDEOWIDGET_H
