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


#ifndef CLSSIS3820COMPOSITESCALERCHANNELVIEW_H
#define CLSSIS3820COMPOSITESCALERCHANNELVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QHBoxLayout>

class CLSSIS3820ScalerChannel;

#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "ui/beamline/AMCurrentAmplifierCompositeView.h"

/// This view mimics the standard CLSSIS3820ScalerChannelView, but uses two scaler channels instead of one.
class CLSSIS3820CompositeScalerChannelView : public QWidget
{
	Q_OBJECT

public:
	/// Simple enum that enumerates the two modes that the output can be viewed.
	enum OutputViewMode { Counts = 0, Voltage = 1 };

	/// Constructor.  Takes two CLSSIS3820ScalerChannels and builds a view around them.
	explicit CLSSIS3820CompositeScalerChannelView(CLSSIS3820ScalerChannel *channel1, CLSSIS3820ScalerChannel *channel2, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSSIS3820CompositeScalerChannelView();

signals:
	/// Notifier that the channel changed viewing mode.  Passes the new mode.
	void outputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode);
	/// Notifier that passes along that the SR570 view changed.
	void amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode);

public slots:
	/// Sets the custom name for the composite scaler channel view.
	void setCustomCompositeName(const QString &name);
	/// Convenience method that sets the output view mode for the channel.
	void setOutputViewMode(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode);
	/// Convenience method that sets the view mode of the SR570.
	void setAmplifierViewMode(AMCurrentAmplifierView::ViewMode mode);
	/// Sets the visibility for the enable check box.
	void setEnableCheckBoxVisibility(bool visible);
	/// Sets the custom name visibility.
	void setCustomNameVisibility(bool visible);
	/// Sets the SR570 visibility (does nothing if no SR570 is set).
	void setAmplifierVisibility(bool visible);
	/// Sets the output visibility.
	void setOutputVisibility(bool visible);
	/// Sets the status label visibility.
	void setStatusLabelVisibility(bool visible);
	/// Sets the precision for the composite view.
	void setAmplifierViewPrecision(int newPrecision);
	/// Sets the format for the composite view.
	void setAmplifierViewFormat(char newFormat);

protected slots:
	/// Handles building the appropriate string when the reading changed.
	void onReadingChanged();
	/// Handles changing the view mode when the scaler output button is clicked.
	void onScalerOutputClicked();
	/// Handles updating the status label.  Includes the color and updating the tool tip.
	void updateStatusLabel();
	/// Handles setting up the SR570 view if it is added after the channel has been set inside the view.
	void onNewCurrentAmplifierAttached();
	/// Handles updating the name label when switching between views modes.
	void updateCompositeNameLabel();
	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);

protected:
	/// Pointer to the first channel being viewed.
	CLSSIS3820ScalerChannel *channel1_;
	/// Pointer to the second channel being viewed.
	CLSSIS3820ScalerChannel *channel2_;

	/// Flag holding whether the view should show the individual elements or the composite view.
	bool showCompositeView_;
	/// The custom name to be given to the composite scaler channel.
	QString compositeName_;

	/// Label holding the current reading of the scaler channel.  Depending on it's state, will display either counts or voltage.
	QToolButton *scalerOutput_;
	/// The check box showing whether the channel is enabled or not.
	QCheckBox *enableBox_;
	/// Flag holding what the output should be.
	OutputViewMode viewMode_;
	/// Label holding the name of the scaler channel.
	QLabel *channelName_;
	/// Holds the SR570, should there be a SR570 bound to this channel.
	AMCurrentAmplifierCompositeView *amplifierView_;
	/// Holds the label for the status.
	QLabel *statusLabel_;
	/// The layout that holds all elements of the view.
	QHBoxLayout *channelLayout_;
};

#endif // CLSSIS3820COMPOSITESCALERCHANNELVIEW_H
