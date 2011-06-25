#ifndef AMOVERLAYVIDEOWIDGET_H
#define AMOVERLAYVIDEOWIDGET_H

#include <QGraphicsView>
#include <QGraphicsVideoItem>

/// This is a convenience class that embeds a QGraphicsVideoItem inside a QGraphicsScene, so that other QGraphicsItems can be painted on top of it.
/*! The class itself is a QGraphicsView widget, and it creates an internal QGraphicsScene which you can access via scene().  A QGraphicsVideoItem is created automatically and added to the scene; you can access the underlying video widget using videoWidget().

  <b>Resizing</b>

  When this widget is resized, the scene inside the view it will be resized to keep the same size as the whole widget.  The internal videoWidget() will also be resized to fill the entire scene.

  */
class AMOverlayVideoWidget : public QGraphicsView
{
	Q_OBJECT
public:
	explicit AMOverlayVideoWidget(QWidget *parent = 0);
	~AMOverlayVideoWidget();

	/// Access the QVideoWidget embedded inside this QGraphicsView/scene.  (QVideoWidget is part of the QMultimediaKit module in the QtMobility add-on component. It may be included in Qt proper in 4.8 or 5.  See http://doc.qt.nokia.com/qtmobility-1.1.3/multimedia.html for details.)
	QGraphicsVideoItem* videoItem() { return videoItem_; }

signals:

public slots:

protected:
	/// When this widget is resized, the scene inside the view it will be resized to keep the same size as the whole widget.  The internal videoItem() will also be resized to fill the entire scene.
	void resizeEvent(QResizeEvent *event);

	QGraphicsVideoItem* videoItem_;

};

#endif // AMOVERLAYVIDEOWIDGET_H
