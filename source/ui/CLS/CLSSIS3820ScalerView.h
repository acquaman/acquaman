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


#ifndef CLSSIS3820SCALERVIEW_H
#define CLSSIS3820SCALERVIEW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>
#include <QToolButton>
#include <QCheckBox>
#include <QHBoxLayout>

class CLSSIS3820Scaler;
class CLSSIS3820ScalerChannel;

#include "source/ui/beamline/AMCurrentAmplifierSingleView.h"
#include "ui/CLS/CLSSIS3820ScalerControlsView.h"

/*!
  This class builds a view around a CLSSIS3820ScalerChannel.  It provides a simple view that has a check box
  and the current reading.
  */

class CLSSIS3820ScalerChannelView : public QWidget
{
	Q_OBJECT

public:
	/// Simple enum that enumerates the two modes that the output can be viewed.
	enum OutputViewMode { Counts = 0, Voltage = 1 };

	/// Constructor.  Takes a CLSSIS3820ScalarChannel and builds a view around it.
	CLSSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);
	virtual ~CLSSIS3820ScalerChannelView();

	/// Whether or not the view has a connected AmplifierView
	bool hasAmplifierView() const;
signals:
	/// Notifier that the channel changed viewing mode.  Passes the new mode.
	void outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode);
	/// Notifier that passes along that the amplifier view has changed.
	void amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode);

public slots:
	/// Convenience method that sets the output view mode for the channel.
	virtual void setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode mode);
	/// Convenience method that sets the view mode of the current amplifier.
	void setAmplifierViewMode(AMCurrentAmplifierView::ViewMode newMode);
	/// Sets the visibility for the enable check box.
	void setEnableCheckBoxVisibility(bool visible);
	/// Sets the custom name visibility.
	void setCustomNameVisibility(bool visible);
	/// Sets the SR570 visibility (does nothing if no SR570 is set).
	void setSR570Visibility(bool visible);
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
	virtual void onReadingChanged();
	/// Handles changing the view mode when the scaler output button is clicked.
	void onScalerOutputClicked();
	/// Handles updating the status label.  Includes the color and updating the tool tip.
	void updateStatusLabel();
	/// Handles setting up the SR570 view if it is added after the channel has been set inside the view.
	void onNewCurrentAmplifierAttached();

protected:
	/// Pointer to the channel being viewed.
	CLSSIS3820ScalerChannel *channel_;

	/// Label holding the current reading of the scaler channel.  Depending on it's state, will display either counts or voltage.
	QToolButton *scalerOutput_;
	/// The check box showing whether the channel is enabled or not.
	QCheckBox *enableBox_;
	/// Flag holding what the output should be.
	OutputViewMode viewMode_;
	/// Label holding the name of the scaler channel.
	QLabel *channelName_;
	/// Holds the SR570, should there be a SR570 bound to this channel.
	AMCurrentAmplifierSingleView *amplifierView_;
	/// Holds the label for the status.
	QLabel *statusLabel_;
	/// The layout that holds all elements of the view.
	QHBoxLayout *channelLayout_;
};

/*!
	This class builds a view for the SIS3820 scalar used throughout the CLS.  It takes a scalar object and builds a standard view
	around it that currently is modeled after the current Scaler app view.
  */
class CLSSIS3820ScalerView : public QWidget
{

Q_OBJECT

public:
	/// Constructor.  Takes a scalar object.
	CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
	virtual ~CLSSIS3820ScalerView();

	/// Sets the precision for the composite view.
	void setAmplifierViewPrecision(int newPrecision);
	/// Sets the format for the composite view.
	void setAmplifierViewFormat(char newFormat);

protected slots:
	/// Handles switching all the SR570 views to match the most recent change.
	void onAmplifierViewModeChanged(AMCurrentAmplifierView::ViewMode mode);
	/// Handles switching all the output views to match the most recent change.
	void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode);

protected:
	/// Pointer to the scalar being viewed.
	CLSSIS3820Scaler *scaler_;

	/// The scaler controls view.
	CLSSIS3820ScalerControlsView *controlsView_;
	/// Holds all of the individual channel views.
	QList<CLSSIS3820ScalerChannelView *> channelViews_;

	/// A layout for all of the channel views
	QVBoxLayout *channelLayout_;
	/// The main layout for the widget
	QVBoxLayout *mainVL_;
};

#endif // CLSSIS3820SCALERVIEW_H
