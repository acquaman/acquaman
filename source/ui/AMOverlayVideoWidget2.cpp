#include "AMOverlayVideoWidget2.h"
#include <QResizeEvent>

#include <QGLWidget>

#include <QGraphicsVideoItem>
#include <QLayout>
#include <QDebug>

AMOverlayVideoWidget2::AMOverlayVideoWidget2(QWidget *parent, bool useOpenGlViewport) :
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

	videoItem_ = new QGraphicsVideoItem();
	mediaPlayer_ = new QMediaPlayer();
	mediaPlayer_->setVideoOutput(videoItem_);
	videoItem_->setAspectRatioMode(Qt::KeepAspectRatio);

	scene()->addItem(videoItem_);
	resize(size());

	// Widgets on top!
//    QPushButton* test = new QPushButton("Help!");
//    scene()->addWidget(test);

}

AMOverlayVideoWidget2::~AMOverlayVideoWidget2() {
	// necessary to avoid a bug on Mac, where if the video widget is deleted while the mediaPlayer is still playing, and then the media player is deleted (also while still playing), there is a crash in:
	/* QT7MovieViewRenderer::setupVideoOutput() from
  QT7PlayerSession::~Qt7PlayerSession() from
  QMediaService::~QMediaService()
  QMediaPlayer::~QMediaPlayer()
  */
	mediaPlayer_->setMedia(QMediaContent());

	delete videoItem_;
	delete mediaPlayer_;
}

QSize AMOverlayVideoWidget2::sizeHint() const{
	return QSize(640, 512);
}

void AMOverlayVideoWidget2::resizeEvent(QResizeEvent *event)
{
	if (scene()) {
                scene()->setSceneRect(QRect(QPoint(0, 0), size()));
                videoItem_->setSize(size());

        }
	QGraphicsView::resizeEvent(event);
}
