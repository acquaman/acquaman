/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMBEAMLINECAMERABROWSER_H
#define AMBEAMLINECAMERABROWSER_H

#include "ui/AMBeamlineCameraWidget.h"

class QComboBox;
class QCheckBox;
class AMCrosshairOverlayVideoWidget;
class AMColorPickerButton;
class QSlider;

/// This class extends AMBeamlineCameraWidget with an address bar and history list to browse/explore different media sources.  It provides a good standalone video display when you don't know the video sources that users will want to access. (See the AcquaCam application for an example.)
class AMBeamlineCameraBrowser : public AMBeamlineCameraWidget
{
	Q_OBJECT
public:
	/// Constructor. OpenGL is enabled by default; you can set \c useOpenGlViewport to false if you cannot use OpenGl, but processor usage will be much higher.
 	virtual ~AMBeamlineCameraBrowser();
	AMBeamlineCameraBrowser(QWidget *parent = 0, bool useOpenGlViewport = true);

	/// Returns a list of the URLs of all the video sources in the history (including the current one)
	QStringList previousSourceURLs() const;

	/// Returns the current video source URL that is currently playing (or loading, or attempting to play, etc.)
	QString currentSourceURL() const;

signals:

public slots:

	/// Set the history list of previous sources (URLs of cameras or files visited).
	/*! If there is currently a source playing, it will be left as the most recent item in the new list.*/
	void setPreviousSourceURLs(const QStringList& sourceURLs);

	/// Add a new video source URL and start playing it, or switch to an existing source URL (if this already exists in the history)
	void setCurrentSourceURL(const QString& sourceURL);


protected:

	QComboBox* sourceComboBox_;

protected slots:

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(int index);

	/// Called when the media player has an error (ex: invalid URL specified)
	void onMediaPlayerError();

};

#endif // AMBEAMLINECAMERABROWSER_H
