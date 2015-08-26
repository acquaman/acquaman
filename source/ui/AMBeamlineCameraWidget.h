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


#ifndef AMBEAMLINECAMERAWIDGET_H
#define AMBEAMLINECAMERAWIDGET_H

#include <QWidget>
#include <QUrl>

class QComboBox;
class QCheckBox;
class AMCrosshairOverlayVideoWidget;
class AMColorPickerButton;
class QSlider;
class QPushButton;
class QLabel;

/// This class provides a general-purpose widget that people can use to monitor the video from different network camera sources.
class AMBeamlineCameraWidget : public QWidget
{
	Q_OBJECT
public:
	/// Constructor. OpenGL is enabled by default. Tou can set \c useOpenGlViewport to false if you cannot use OpenGl, but processor usage will be much higher.
 	virtual ~AMBeamlineCameraWidget();
	explicit AMBeamlineCameraWidget(QWidget *parent = 0, bool useOpenGlViewport = true);

	/// Set the video source.
	void playSource(const QUrl& sourceUrl);
	/// Set the video source. Anything that can be converted into a URL of some kind works here. Returns false if given an invalid URL. (Note, however, that this doesn't mean the source was sucessfully opened. We still need to access the network, so success or failure will come asynchronously.)
	bool playSource(const QString& sourceUrl);

	/// Returns the video source
	QUrl source() const;

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
	/// sets a new crosshair center position
	void setCrosshairCenterPosition(const QPointF& pos);

protected:

	AMCrosshairOverlayVideoWidget* videoWidget_;
	QCheckBox* showCrosshairCheckBox_, *lockCrosshairCheckBox_;
	AMColorPickerButton* crosshairColorPicker_;
	QSlider* crosshairThicknessSlider_;
	QPushButton *recenterCrosshairPushbutton_;
	QLabel *colorLabel_, *lineLabel_;

	bool crosshairLocked_;
	QPointF crosshairCenter_;

protected slots:

	/// Called when double-clicking on the video widget (move the cursor if not locked)
	void onVideoWidgetDoubleClicked(const QPointF& clickPoint);
	/// Reset the crosshair to the defined crosshairCenter
	void onRecenterCrosshairPushbuttonClicked();

};

#endif // AMBEAMLINECAMERAWIDGET_H
