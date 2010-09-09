#include "CamWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>

#include <QImage>

#include <QDebug>


AMVideoPlayerWidget::AMVideoPlayerWidget(QWidget *parent) : QFrame(parent) {

	setMinimumSize(100,100);

	/// Create a vlc instance for this widget
	vlcInstance_ = libvlc_new (0, NULL);
	/// create the vlc player itself
	vlcPlayer_ = libvlc_media_player_new(vlcInstance_);
	/// register callbacks so that we can implement custom drawing of video frames
	libvlc_video_set_callbacks(
			vlcPlayer_,
			&lockCBWrapper,
			&unlockCBWrapper,
			&displayCBWrapper,
			this);

	scaleMode_ = Qt::KeepAspectRatio;
	sizeCheck();

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	setStyleSheet("background-color: black;");
}

AMVideoPlayerWidget::~AMVideoPlayerWidget() {
	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_release(vlcPlayer_);
	libvlc_release(vlcInstance_);
}

bool AMVideoPlayerWidget::openVideoUrl(const QString &videoUrl) {

	libvlc_media_t* media = libvlc_media_new_path( vlcInstance_, videoUrl.toAscii().constData() );
	if(media == 0)
		return false;

	/// \todo Look at:
	libvlc_state_t state = libvlc_media_get_state(media);


	libvlc_media_player_set_media(vlcPlayer_, media);
	libvlc_media_release(media);

	setSizeCheckRequired();

	return true;

}




void AMVideoPlayerWidget::play() {
	libvlc_media_player_play(vlcPlayer_);
}

void AMVideoPlayerWidget::pause() {
	libvlc_media_player_pause(vlcPlayer_);
}

void AMVideoPlayerWidget::stop() {
	libvlc_media_player_set_pause(vlcPlayer_, true);
}


/// Called to determine the pixel dimensions of the video source, and determine how big our video display should be
void AMVideoPlayerWidget::sizeCheck() {

	QMutexLocker locker(&sizeCheckMutex_);

	sizeCheckRequired_ = false;

	unsigned int w, h;
	if(libvlc_video_get_size(vlcPlayer_, 0, &w, &h) == 0) {	// returns 0 on success (ie: video loaded and playing)
		videoSourceSize_ = QSize(w,h);
	}
	else {
		videoSourceSize_ = QSize(320, 240);	// doesn't matter for now... we just need something. We'll check again once actual video is playing
	}

	double sourceAspectRatio = double(videoSourceSize_.height()) / videoSourceSize_.width();
	double displayAspectRatio = double(height())/width();

	switch(scaleMode_) {
	case Qt::IgnoreAspectRatio:
		videoDisplayRect_ = QRect(0,0,width(),height());	// in IgnoreAspectRatio mode, we just render to fill the whole widget
		break;
	case Qt::KeepAspectRatio:	// In this mode, we shrink video to fit, with the original aspect ratio
		if(displayAspectRatio >= sourceAspectRatio) { // our display is taller than the source. letterbox above and below; video goes full width
			int displayHeight = width()*sourceAspectRatio;
			qDebug() << "displayHeight:" << displayHeight;
			videoDisplayRect_ = QRect(0, (height()-displayHeight)/2, width(), displayHeight);
		}
		else {
			// our display is fatter than the source. Letterbox on sides; video goes full height
			int displayWidth = height()*sourceAspectRatio;
			qDebug() << "displayWidth:" << displayWidth;
			videoDisplayRect_ = QRect((width()-displayWidth)/2, 0, displayWidth, height());
		}
		break;
	case Qt::KeepAspectRatioByExpanding: // in this mode, we expand video and crop on edges if required
		if(displayAspectRatio >= sourceAspectRatio) { // our display is taller than the source. Video goes full height; width is over-sized.
			int displayWidth = height()*sourceAspectRatio;
			videoDisplayRect_ = QRect((width()-displayWidth)/2, height(), displayWidth, height());
		}
		else {
			// our display is fatter than the source. Video goes full-width; height is over-sized.
			int displayHeight = width()*sourceAspectRatio;
			videoDisplayRect_ = QRect(0, (height()-displayHeight)/2, width(), displayHeight);
		}
		break;
	}


	/// What is the "line pitch"? Looks like it's the number of width pixels X the size of a pixel (in bytes...)
	libvlc_video_set_format(vlcPlayer_, "RV32", videoDisplayRect_.width(), videoDisplayRect_.height(), videoDisplayRect_.width()*4);

	/// Check what happened:
	qDebug() << "SSSSSSSSSSize CCCCCCCCCCCCheck!!!!!!!!!  source size:" << videoSourceSize_ << "display rect:" << videoDisplayRect_;

}

/// Called when a video frame needs to be decoded; this must allocate video memory, which is returned in the argument \c pixelPlane. Also returns a pointer that will be passed into unlockCB() and displayCB() as the void* \c picture argument.
void* AMVideoPlayerWidget::lockCB(void** pixelPlane) {

	if(sizeCheckRequired()) {
		sizeCheck();
	}

	sizeCheckMutex_.lock();
	QSize videoSize = videoDisplayRect_.size();
	sizeCheckMutex_.unlock();


	renderPixmapMutex_.lock();
	if(renderPixmap_.size() != videoSize)
		renderPixmap_ = QImage(videoSize.width(), videoSize.height(), QImage::QImage::Format_RGB32);
	*pixelPlane = renderPixmap_.bits();
	return &renderPixmap_;

}

/// Called when a video frame is done decoding; this can free the memory. \c picture is the return value from lockCB().
void AMVideoPlayerWidget::unlockCB(void* picture, void*const *pixelPlane) {
	renderPixmapMutex_.unlock();
}

/// Called when a video frame is ready to be displayed, according to the vlc clock. \c picture is the return value from lockCB().
void AMVideoPlayerWidget::displayCB(void* picture) {


	renderPixmapMutex_.lock();
	onScreenPixmapMutex_.lock();
	onScreenPixmap_ = QPixmap::fromImage(renderPixmap_);
	onScreenPixmapMutex_.unlock();
	renderPixmapMutex_.unlock();

	update();
}


void AMVideoPlayerWidget::paintEvent(QPaintEvent *e) {
	QFrame::paintEvent(e);

	sizeCheckMutex_.lock();
	QRect displayRect = videoDisplayRect_;
	sizeCheckMutex_.unlock();

	QPainter p(this);
	onScreenPixmapMutex_.lock();
	p.drawPixmap(displayRect, onScreenPixmap_, onScreenPixmap_.rect());
	onScreenPixmapMutex_.unlock();
	p.end();


}

CamWidget::CamWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent) : QWidget(parent) {

	// Setup UI:
	/////////////////////

	// Layouts: full vertical, with horizontal sub-layout at bottom.
	QVBoxLayout* vl1 = new QVBoxLayout(this);
	QHBoxLayout* hl1 = new QHBoxLayout();

	// VideoWidget takes up main space:
	videoWidget_ = new AMVideoPlayerWidget(this);

	vl1->addWidget(videoWidget_);

	// add horizontal layout at bottom:
	vl1->addLayout(hl1);

	hl1->addStretch();
	QLabel* label = new QLabel("Source");
	hl1->addWidget(label);

	// Add combobox with list of camera names/addresses:
	cameraList_ = new QComboBox();
	hl1->addWidget(cameraList_);

	cameraList_->insertItem(0, cameraName, cameraAddress);
	connect(cameraList_, SIGNAL(activated(int)), this, SLOT(onSourceChanged(int)));
	onSourceChanged( cameraList_->currentIndex() );


}

CamWidget::~CamWidget()
{
}


#include "AMErrorMonitor.h"

void CamWidget::onSourceChanged(int index) {

	videoWidget_->stop();

	QUrl source( cameraList_->itemData(index).toUrl() );

	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, QString("Attempting to connect to video server: '%1%2' with user name: '%3' and password: '%4'").arg(source.host()).arg(source.path()).arg(source.userName()).arg(source.password())));

	videoWidget_->openVideoUrl( source.toString() );

	videoWidget_->play();

}

void CamWidget::addSource(const QString& cameraName, const QUrl& cameraAddress) {

	cameraList_->addItem(cameraName, QVariant::fromValue(cameraAddress));
}
