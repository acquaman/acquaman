#ifndef AMOVERLAYVIDEOWIDGET_H
#define AMOVERLAYVIDEOWIDGET_H

#include <QGraphicsView>
#include <QResizeEvent>
#include <phonon/VideoPlayer>

class AMOverlayVideoWidget : public QGraphicsView
{
	Q_OBJECT
public:
	explicit AMOverlayVideoWidget(QWidget *parent = 0);

	Phonon::VideoPlayer* videoPlayer() { return videoPlayer_; }

signals:

public slots:

protected:
	void resizeEvent(QResizeEvent *event) {
		if (scene()) {
			scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
			videoPlayer_->setGeometry(QRect(QPoint(0,0), event->size()));
		}
		QGraphicsView::resizeEvent(event);
	}

	Phonon::VideoPlayer* videoPlayer_;

};

#endif // AMOVERLAYVIDEOWIDGET_H
