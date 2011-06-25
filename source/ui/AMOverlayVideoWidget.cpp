#include "AMOverlayVideoWidget.h"
#include <QResizeEvent>

//#include <QGLWidget>

#include <QPushButton>
#include <QMediaPlayer>

AMOverlayVideoWidget::AMOverlayVideoWidget(QWidget *parent) :
	QGraphicsView(parent)
{

	//setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
	//setViewport(new QGLWidget());
	//setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setScene(new QGraphicsScene());

	videoItem_ = new QGraphicsVideoItem();
	scene()->addItem(videoItem_);

	resize(size());

	QPushButton* test = new QPushButton("Help!");
	scene()->addWidget(test);

}

AMOverlayVideoWidget::~AMOverlayVideoWidget() {
	// necessary to avoid a bug on Mac, where if the video widget is deleted while the mediaPlayer is still playing, and then the media player is deleted (also while still playing), there is a crash in:
	/* QT7MovieViewRenderer::setupVideoOutput() from
		QT7PlayerSession::~Qt7PlayerSession() from
		QMediaService::~QMediaService()
		QMediaPlayer::~QMediaPlayer()
		*/
	QMediaPlayer* player  = qobject_cast<QMediaPlayer*>(videoItem_->mediaObject());
	if(player) {
		player->setMedia(QMediaContent());
	}

	delete videoItem_;
}

void AMOverlayVideoWidget::resizeEvent(QResizeEvent *event)
{
	if (scene()) {
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
		videoItem_->setSize(event->size());
		//videoItem_->setGeometry(QRect(QPoint(0,0), event->size()));
	}
	QGraphicsView::resizeEvent(event);
}

