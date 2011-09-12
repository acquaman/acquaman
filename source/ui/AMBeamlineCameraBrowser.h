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


#ifndef AMBEAMLINECAMERABROWSER_H
#define AMBEAMLINECAMERABROWSER_H

#include <QWidget>

class QComboBox;
class QCheckBox;
class AMCrosshairOverlayVideoWidget;
class AMColorPickerButton;
class QSlider;

/// This class provides a general-purpose widget that people can use to monitor the video from different network camera sources.
class AMBeamlineCameraBrowser : public QWidget
{
	Q_OBJECT
public:
	explicit AMBeamlineCameraBrowser(QWidget *parent = 0, bool useOpenGlViewport = true);


	/// Is the crosshair locked?
	bool crosshairLocked() const { return crosshairLocked_; }
	/// Returns the crosshair color
	QColor crosshairColor() const;
	/// Returns the crosshair line thickness
	int crosshairLineThickness() const;
	/// Is the crosshair visible?
	bool crosshairVisible() const;
	/// Returns the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
	QPointF crosshairPosition() const;

	/// Returns a list of the URLs of all the video sources in the history (including the current one)
	QStringList previousSourceURLs() const;

	/// Returns the current video source URL that is currently playing (or loading, or attempting to play, etc.)
	QString currentSourceURL() const;

signals:

public slots:

	/// Set the crosshair color
	void setCrosshairColor(const QColor& color);
	/// Set the crosshair line thickness
	void setCrosshairLineThickness(int thickness);
	/// Set whether the crosshair is visible or not
	void setCrosshairVisible(bool isVisible);
	/// Disable the capability to move the cross-hair by double-clicking
	void setCrosshairLocked(bool doLock = true);
	/// Set the crosshair position (relative X-Y position on the video display, from (0,0)[top left] to (1,1)[bottom right] )
	void setCrosshairPosition(const QPointF& pos) const;

	/// Set the history list of previous sources (URLs of cameras or files visited).
	/*! If there is currently a source playing, it will be left as the most recent item in the new list.*/
	void setPreviousSourceURLs(const QStringList& sourceURLs);

	/// Add a new video source URL and start playing it, or switch to an existing source URL (if this already exists in the history)
	void setCurrentSourceURL(const QString& sourceURL);


protected:

	AMCrosshairOverlayVideoWidget* videoWidget_;
	QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
	AMColorPickerButton* crosshairColorPicker_;
	QComboBox* sourceComboBox_;
	QSlider* crosshairThicknessSlider_;

	bool crosshairLocked_;


protected slots:

	/// called when the user selects a different source from the combobox
	void onSourceComboBoxChanged(int index);

	/// Called when double-clicking on the video widget (move the cursor if not locked)
	void onVideoWidgetDoubleClicked(const QPointF& clickPoint);

	/// Called when the media player has an error (ex: invalid URL specified)
	void onMediaPlayerError();

};

#endif // AMBEAMLINECAMERABROWSER_H
