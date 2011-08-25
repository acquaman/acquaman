/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
