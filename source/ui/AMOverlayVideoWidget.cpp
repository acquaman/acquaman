#include "AMOverlayVideoWidget.h"
#include <QResizeEvent>

#include <QGLWidget>

#include <QGraphicsVideoItem>
#include <QVideoWidget>


AMOverlayVideoWidget::AMOverlayVideoWidget(QWidget *parent, bool useOpenGlViewport) :
	QGraphicsView(parent)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

	if(useOpenGlViewport) {
		// setViewport(new QGLWidget());
		setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
		setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}

	setScene(new QGraphicsScene());
	scene()->setBackgroundBrush(QBrush(QColor(50,50,50)));

	// videoItem_ = new QGraphicsVideoItem();
	videoWidget_ = new QVideoWidget();
	mediaPlayer_ = new QMediaPlayer();
	mediaPlayer_->setVideoOutput(videoWidget_);

	//scene()->addItem(videoItem_);
	// scene()->addWidget(videoWidget_);
	videoWidget_->show();
	resize(size());

	// Widgets on top!
//	QPushButton* test = new QPushButton("Help!");
//	scene()->addWidget(test);

}

AMOverlayVideoWidget::~AMOverlayVideoWidget() {
	// necessary to avoid a bug on Mac, where if the video widget is deleted while the mediaPlayer is still playing, and then the media player is deleted (also while still playing), there is a crash in:
	/* QT7MovieViewRenderer::setupVideoOutput() from
  QT7PlayerSession::~Qt7PlayerSession() from
  QMediaService::~QMediaService()
  QMediaPlayer::~QMediaPlayer()
  */
	mediaPlayer_->setMedia(QMediaContent());

	//delete videoItem_;
	delete videoWidget_;
	delete mediaPlayer_;
}

void AMOverlayVideoWidget::resizeEvent(QResizeEvent *event)
{
	if (scene()) {
		scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
		//videoWidget_->setSize(event->size());
		videoWidget_->setGeometry(QRect(QPoint(0,0), event->size()));
	}
	QGraphicsView::resizeEvent(event);
}

