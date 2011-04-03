#include "ui/AMVideoWidget.h"

bool AMVideoWidget::openVideoUrl(const QString &videoUrl) {

	libvlc_media_t* media = libvlc_media_new_path( vlcInstance_, videoUrl.toAscii().constData() );
	if(media == 0)
		return false;

	/// \todo Look at:
	// libvlc_state_t state = libvlc_media_get_state(media);

	libvlc_media_player_stop(vlcPlayer_);
	libvlc_media_player_set_media(vlcPlayer_, media);
	libvlc_media_release(media);

	return true;
}
