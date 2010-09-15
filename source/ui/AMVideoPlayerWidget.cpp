#include "AMVideoPlayerWidget.h"

#include <QImage>
#include <QPalette>
#include <QPainter>

#include <QDebug>



AMVideoPlayerWidget::AMVideoPlayerWidget(QWidget *parent) : QFrame(parent) {

	setMinimumSize(100,100);

	/// Create a vlc instance for this widget
	vlcInstance_ = libvlc_new (0, NULL);
	/// create the vlc player itself
	vlcPlayer_ = libvlc_media_player_new(vlcInstance_);


	sourceSizeCheckRequired_ = false;
	videoSourceSize_ = QSize(640, 480);	// doesn't matter... we just need something for now. We'll pick this up again once we get actual video.
	libvlc_video_set_format(vlcPlayer_, "RV32", videoSourceSize_.width(), videoSourceSize_.height(), videoSourceSize_.width()*4);

	// compute initial display size
	scaleMode_ = Qt::KeepAspectRatio;
	displaySizeHelper();

	/// register callbacks so that we can implement custom drawing of video frames
	libvlc_video_set_callbacks(
			vlcPlayer_,
			&lockCBWrapper,
			&unlockCBWrapper,
			&displayCBWrapper,
			this);

	QPalette p = palette();
	p.setBrush(QPalette::Window, Qt::black);
	setPalette(p);
	setAutoFillBackground(true);
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
	// libvlc_state_t state = libvlc_media_get_state(media);


	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_set_media(vlcPlayer_, media);
	libvlc_media_release(media);

	sourceSizeCheckRequired_ = true;

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


void AMVideoPlayerWidget::videoResizeEvent(AMVideoResizeEvent* event) {

	if(!sourceSizeCheckRequired_)
		return;

	sourceSizeCheckRequired_ = false;
	event->accept();

	libvlc_media_player_stop(vlcPlayer_);

	videoSourceSizeMutex_.lock();	// this critical section ensures the videoSourceSize_ and the libvlc render format size are always in sync. The lockCB() function called from the vlc thread will need to wait for this to finish before being able to access videoSourceSize().
	videoSourceSize_ = event->newSize();
	libvlc_video_set_format(vlcPlayer_, "RV32", videoSourceSize_.width(), videoSourceSize_.height(), videoSourceSize_.width()*4);
	videoSourceSizeMutex_.unlock(); // end critical section.

	qDebug() << "VIDEO SOURCE RESIZE EVENT. Re-scaling and changing format to new size:" << videoSourceSize_;

	displaySizeHelper();
	libvlc_media_player_play(vlcPlayer_);
}

/// Called to determine the pixel dimensions of the video source, and determine how big our video display should be
void AMVideoPlayerWidget::displaySizeHelper() {

	QSize sourceSize = videoSourceSize();

	double sourceAspectRatio = double(sourceSize.height()) / sourceSize.width();
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

	qDebug() << "VIDEO DISPLAY SIZE ADJUSTED ======  source size:" << sourceSize << "display rect:" << videoDisplayRect_;
}

#include <QApplication>
/// Called when a video frame needs to be decoded; this must allocate video memory, which is returned in the argument \c pixelPlane. Also returns a pointer that will be passed into unlockCB() and displayCB() as the void* \c picture argument.
void* AMVideoPlayerWidget::lockCB(void** pixelPlane) {

	QSize sourceSize = videoSourceSize();

	if(sourceSizeCheckRequired_) {
		unsigned int w,h;
		if(libvlc_video_get_size(vlcPlayer_, 0, &w, &h) == 0)	// returns 0 on success (ie: media loaded and playing... we now know the real size)
			QApplication::postEvent(this, new AMVideoResizeEvent(QSize(w,h), sourceSize));
	}

	QImage* image = new QImage(sourceSize.width(), sourceSize.height(), QImage::Format_RGB32);

	*pixelPlane = image->bits();
	return image;

}

/// Called when a video frame is done decoding; this can free the memory. \c picture is the return value from lockCB().
void AMVideoPlayerWidget::unlockCB(void* picture, void*const *pixelPlane) {
	Q_UNUSED(picture)
	Q_UNUSED(pixelPlane)
}

/// Called when a video frame is ready to be displayed, according to the vlc clock. \c picture is the return value from lockCB().
void AMVideoPlayerWidget::displayCB(void* picture) {

	QImage* image = reinterpret_cast<QImage*>(picture);

	onScreenPixmapMutex_.lock();
	onScreenPixmap_ = QImage(*image);
	onScreenPixmap_.detach();
	onScreenPixmapMutex_.unlock();

	delete image;

	update();
}


void AMVideoPlayerWidget::paintEvent(QPaintEvent *e) {
	QFrame::paintEvent(e);

	QPainter p(this);
	onScreenPixmapMutex_.lock();
	p.drawImage(videoDisplayRect_, onScreenPixmap_, onScreenPixmap_.rect());
	onScreenPixmapMutex_.unlock();
	p.end();


}
