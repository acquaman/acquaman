#ifndef ACQMAN_CAMWIDGET_H_
#define ACQMAN_CAMWIDGET_H_

#include <QWidget>
#include <QComboBox>

//#include <mediaobject.h>
//#include <videowidget.h>
#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

/// A replacement for Phonon::VideoWidget that can display a cross-hair and emit signals when clicked in a specific position.
/*
class AMCrosshairVideoWidget : public Phonon::VideoWidget {

	Q_OBJECT

public:
	AMCrosshairVideoWidget(QWidget* parent = 0)
		: Phonon::VideoWidget(parent) {

		crosshairX_ = 0.5;
		crosshairY_ = 0.5;
		crosshairPen_ = QPen(QColor(Qt::red));
		crosshairVisible_ = true;
	}

public:
	QPointF crosshairPos() const { return QPointF(crosshairX_, crosshairY_); }
	QPen crosshairPen() const { return crosshairPen_; }
	bool crosshairVisible() const { return crosshairVisible_; }

public slots:
	void setCrosshairPos(const QPointF& pos) {
		crosshairX_ = pos.x();
		crosshairY_ = pos.y();
		update();
	}

	void setCrosshairPen(const QPen& pen) {
		crosshairPen_ = pen;
		update();
	}

	void showCrosshair(bool crosshairVisible = true) {
		crosshairVisible_ = crosshairVisible;
		update();
	}

signals:
	void videoClicked(const QPointF& position);

protected:
	double crosshairX_, crosshairY_;
	QPen crosshairPen_;
	bool crosshairVisible_;

	virtual void paintEvent(QPaintEvent *event) {
		Phonon::VideoWidget::paintEvent(event);

		if(crosshairVisible_) {
			qDebug() << "drawing line...";
			QPainter painter(this);
			painter.setPen(crosshairPen_);
			painter.drawLine(QPointF(width()*crosshairX_, 0), QPointF(width()*crosshairX_, height()));
			painter.drawLine(QPointF(0, height()*crosshairY_), QPointF(width(), height()*crosshairY_));
			painter.end();
		}
	}

	//virtual void

};*/

#include <QFrame>
#include <QMutex>
#include "vlc/libvlc.h"
#include "vlc/libvlc_media.h"
#include "vlc/libvlc_media_player.h"
#include "vlc/libvlc_structures.h"

#define VIDEOWIDTH 1280
#define VIDEOHEIGHT 1024

class AMVideoPlayerWidget : public QFrame {

	Q_OBJECT

public:
	AMVideoPlayerWidget(QWidget* parent = 0);
	virtual ~AMVideoPlayerWidget();

	/// Open a video file or network stream. Returns true on success
	/*! Formats:
	[file://]filename              Plain media file
	http://ip:port/file            HTTP URL
	ftp://ip:port/file             FTP URL
	mms://ip:port/file             MMS URL
	screen://                      Screen capture
	[dvd://][device][@raw_device]  DVD device
	[vcd://][device]               VCD device
	[cdda://][device]              Audio CD device
	udp:[[<source address>]@[<bind address>][:<bind port>]]
	*/
	bool openVideoUrl(const QString& videoUrl);

	/// Start playback
	void play();
	/// Pause playback
	void pause();
	/// Stop playback and rewind to beginning
	void stop();




protected:

	// Callbacks from libvlc video decoding engine:
	////////////////////////////////
	/// Called when a video frame needs to be decoded; this must allocate video memory, which is returned in the argument \c pixelPlane. Also returns a pointer that will be passed into unlockCB() and displayCB() as the void* \c picture argument.
	void* lockCB(void** pixelPlane);

	/// Called when a video frame is done decoding; this can free the memory. \c picture is the return value from lockCB().
	void unlockCB(void* picture, void*const *pixelPlane);

	/// Called when a video frame is ready to be displayed, according to the vlc clock. \c picture is the return value from lockCB().
	void displayCB(void* picture);


	// static callback wrappers. (Since we can't specify a pointer to a member function, these receive a pointer to the correct widget and pass on the function call):
	/// Called when a video frame needs to be decoded
	static void* lockCBWrapper(void* widget, void** pixelPlane) {
		AMVideoPlayerWidget* w = reinterpret_cast<AMVideoPlayerWidget*>(widget);
		return w->lockCB(pixelPlane);
	}

	/// Called when a video frame is done decoding
	static void unlockCBWrapper(void* widget, void* picture, void*const *pixelPlane) {
		AMVideoPlayerWidget* w = reinterpret_cast<AMVideoPlayerWidget*>(widget);
		w->unlockCB(picture, pixelPlane);
	}

	/// Called when a video frame needs to be displayed
	static void displayCBWrapper(void* widget, void* picture) {
		AMVideoPlayerWidget* w = reinterpret_cast<AMVideoPlayerWidget*>(widget);
		w->displayCB(picture);
	}
	//////////////////////////////////


	// Reimplemented paint event:
	virtual void paintEvent(QPaintEvent *e);

protected:
	libvlc_instance_t *vlcInstance_;
	libvlc_media_player_t *vlcPlayer_;

	/// The onScreenPixmap_ caches the video frame for widget painting. The renderPixmap_ is the buffer used by VLC to draw into.
	QPixmap onScreenPixmap_;
	QImage renderPixmap_;

	/// This is the native size of the video:
	QSize videoSourceSize_;
	/// This is the rectangle that we decode the video into, depending on the scaleMode_ and widget size
	QRect videoDisplayRect_;
	/// Call this helper function when the video source or widget size changes, to determine the size of the source video, and calculate (based on scaleMode_) the rectangle into which it gets displayed
	void sizeCheck();
	bool sizeCheckRequired_;

	/// Specifies how the video is scaled when the widget size doesn't match the source video size:
	/*! Qt::IgnoreAspectRatio: ignore aspect ratio and stretch the video to fill the widget
		Qt::KeepAspectRatio: show "letterbox" bars on the sides or top, and keep the entire video at original aspect ratio inside the frame
		Qt::KeepAspectRatioByExpanding: crop the video and display as much as possible on screen, while keeping the original aspect ratio.
		*/
	Qt::AspectRatioMode scaleMode_;

	/// Mutex used to protect access to onScreenPixmap_, which is accessed from both displayCB() and paintEvent().
	/*! Motivation: displayCB() could be called from a separate thread, writing to onScreenPixmap, while the Qt GUI thread is reading onScreenPixmap_ inside paintEvent() */
	QMutex onScreenPixmapMutex_;
	/// Mutex used to protect all variables accessed in sizeCheck_ (sizeCheckRequired_, videoSourceSize_, videoDisplayRect_, and widget geometry height() and width())
	QMutex sizeCheckMutex_;
	/// Mutex used to lock/unlock access to the render pixmap buffer
	QMutex renderPixmapMutex_;

	/// Convenience function to set sizeCheckRequired_ to true. Locks sizeCheckMutex_ before access.
	void setSizeCheckRequired() {
		sizeCheckMutex_.lock();
		sizeCheckRequired_ = true;
		sizeCheckMutex_.unlock();
	}

	/// Convenience function to check if sizeCheckRequired_.  Locks sizeCheckMutex_ before access
	bool sizeCheckRequired() {
		QMutexLocker locker(&sizeCheckMutex_);
		return sizeCheckRequired_;
	}


	/// Resize event: will require a new sizeCheck()
	void resizeEvent(QResizeEvent *e) {
		QMutexLocker locker(&sizeCheckMutex_);
		QFrame::resizeEvent(e);
		sizeCheckRequired_  = true;
	}

};





// A widget that provides a view onto multiple Axis-server MJPEG streams,
// using the Phonon multimedia library

class CamWidget : public QWidget {

	Q_OBJECT

public:
	CamWidget(const QString& cameraName, const QUrl& cameraAddress, QWidget* parent = 0);
	virtual ~CamWidget();

	void setupVideo();

	void addSource(const QString& cameraName, const QUrl& cameraAddress);

public slots:

	void onSourceChanged(int index);


protected:
	// AMCrosshairVideoWidget* videoWidget_;
	// Phonon::MediaObject* mediaObject_;

	AMVideoPlayerWidget* videoWidget_;
	QComboBox* cameraList_;


};

#endif /*CAMWIDGET_H_*/
