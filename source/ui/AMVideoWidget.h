#ifndef AMVIDEOWIDGET_H
#define AMVIDEOWIDGET_H

#include <QFrame>
#include "vlc/libvlc.h"
#include "vlc/libvlc_media.h"
#include "vlc/libvlc_media_player.h"

class QMacCocoaViewContainer;

class AMVideoWidget : public QFrame
{
	Q_OBJECT
public:
	explicit AMVideoWidget(QWidget *parent = 0);
	virtual ~AMVideoWidget();

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

	/// isPlaying is true when video is in playback
	bool isPlaying() const { return libvlc_media_player_is_playing(vlcPlayer_); }
	/// returns the exact state of playback (see
	libvlc_state_t state() const { return libvlc_media_player_get_state(vlcPlayer_); }

signals:

public slots:

	/// Start playback
	void play();

	/// Pause playback
	void pause();
	/// Resume playback
	void unPause();
	/// Convenience function to toggle pause on/off
	void togglePause();

	/// Stop playback and rewind to the beginning
	void stop();

protected:
	QMacCocoaViewContainer* macViewContainer_;

	libvlc_instance_t* vlcInstance_;
	libvlc_media_player_t* vlcPlayer_;

};

#endif // AMVIDEOWIDGET_H
