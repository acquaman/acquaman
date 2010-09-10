#ifndef AMVIDEOPLAYERWIDGET_H
#define AMVIDEOPLAYERWIDGET_H

#include <QEvent>
#include <QFrame>
#include <QMutex>
#include "acquaman.h"
#include "vlc/libvlc.h"
#include "vlc/libvlc_media.h"
#include "vlc/libvlc_media_player.h"
// #include "vlc/libvlc_structures.h"



/*
class AMSimpleImageBuffer {
public:
	enum Format { ARGB32_Format };

	AMSimpleImageBuffer(int w, int h) {
		width = w;
		height = h;
		format = ARGB32_Format;	// for now, only one option
		bytesPerPixel = 4;
		data = new uchar[width*height*bytesPerPixel];
	}

	~AMSimpleImageBuffer() {
		delete [] data;
	}

	uchar* data;
	int width, height;
	int bytesPerPixel;
	Format format;
};*/


/// This event class is used by AMVideoPlayerWidget to pass video source resize notifications from the video library thread to the main Qt runloop thread, using QApplication::postEvent().
class AMVideoResizeEvent : public QEvent {

public:
	AMVideoResizeEvent(const QSize& newSize, const QSize& oldSize)
		: QEvent( (QEvent::Type)AM::VideoResizeEvent ) {

		newSize_ = newSize;
		oldSize_ = oldSize;
	}

	QSize newSize() const { return newSize_; }
	QSize oldSize() const { return oldSize_; }

protected:
	QSize newSize_, oldSize_;
};

/// This widget is a video player that can play pretty much any file or network stream that the VLC movie player can handle.
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
	/// Toggle pause on and off
	void pause();
	/// Stop playback and rewind to the beginning
	void stop();


	/// Set how video is scaled, if the aspect ratio of the widget doesn't match the aspect ratio of the source video. The default is to display at the original aspect ratio, with "letterbox" bars above and below, or on the sides, if required.
	void setScaleMode(Qt::AspectRatioMode scaleMode = Qt::KeepAspectRatio) { scaleMode_ = scaleMode; displaySizeHelper(); }

	/// Size hint: This widget attempts to match the size of its source video, if possible.
	virtual QSize sizeHint() const { return videoSourceSize(); }

	/// Specify if you want the letterbox bars filled in, or transparent
	void setFillBackground(bool backgroundFilled = true) { setAutoFillBackground(backgroundFilled); }



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
	/////////////////////////////////


	/// Reimplemented paint event: paints the latest video frame into the widget, inside videoDisplayRect_
	virtual void paintEvent(QPaintEvent *e);

protected:
	/// VLC library instance and player object
	libvlc_instance_t *vlcInstance_;
	libvlc_media_player_t *vlcPlayer_;

	/// The onScreenPixmap_ caches the latest video frame for widget painting.
	QPixmap onScreenPixmap_;
	/// Mutex used to protect access to onScreenPixmap_, which is accessed from both displayCB() and paintEvent().
	/*! Motivation: displayCB() could be called from a separate thread, writing to onScreenPixmap, while the Qt GUI thread is reading onScreenPixmap_ inside paintEvent() */
	QMutex onScreenPixmapMutex_;


	/// This is the native size of the video. It's read from the main thread, and also read from the VLC thread inside lockCB(). It's written only from the main thread, inside videoResizeEvent().
	QSize videoSourceSize_;
	/// This mutex protects access to videoSourceSize_
	mutable QMutex videoSourceSizeMutex_;
	/// Get the video source size (thread-safed)
	QSize videoSourceSize() const {
		QMutexLocker locker(&videoSourceSizeMutex_);
		return videoSourceSize_;
	}


	/// This flag indicates that we're still waiting to find out the actual size of the video stream. It's written by openVideoUrl when a new stream is opened, and set to false inside the videoResizeEvent() handler (both in the main thread). It's read (read only) from the vlc thread.
	volatile bool sourceSizeCheckRequired_;

	/// Video source resize event handler. Stops the video, sets the VLC render format to match the original source size (for performance), saves this in videoSourceSize_, and plays the video again.  Sets sourceSizeCheckRequired_ to false when complete.  Called only in the main thread; not from the VLC thread.
	void videoResizeEvent(AMVideoResizeEvent* event);




	/// This is the rectangle that we display the video in, depending on the scaleMode_ and widget size. It's in local widget geometry coordinates.
	QRect videoDisplayRect_;
	/// This helper function is called when either the widget size changes, or the source size changes. (Called only within the main thread). It recomputes videoDisplayRect_ based on the aspect ratios, sizes, and scaleMode_)
	void displaySizeHelper();


	/// Specifies how the video is scaled when the widget size doesn't match the source video size:
	/*! Qt::IgnoreAspectRatio: ignore aspect ratio and stretch the video to fill the widget
		Qt::KeepAspectRatio: show "letterbox" bars on the sides or top, and keep the entire video at original aspect ratio inside the frame
		Qt::KeepAspectRatioByExpanding: crop the video and display as much as possible on screen, while keeping the original aspect ratio.
		*/
	Qt::AspectRatioMode scaleMode_;




	/// Resize event: will require a new display size...
	void resizeEvent(QResizeEvent *e) {
		QFrame::resizeEvent(e);

		displaySizeHelper();
	}

	/// Re-implemented event handler to dispatch video source resize events
	bool event(QEvent *e) {
		if(e->type() == (QEvent::Type)AM::VideoResizeEvent) {
			videoResizeEvent(static_cast<AMVideoResizeEvent*>(e));
			return true;
		}
		else {
			return QFrame::event(e);
		}
	}



};


#endif // AMVIDEOPLAYERWIDGET_H
