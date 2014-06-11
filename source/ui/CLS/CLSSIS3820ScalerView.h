/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "ui/CLS/CLSSR570View.h"
#include "ui/CLS/CLSDarkCurrentWidget.h"
#include "source/ui/beamline/AMCurrentAmplifierView.h"

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

signals:
	/// Notifier that the channel changed viewing mode.  Passes the new mode.
	void outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode);
//	/// Notifier that passes along that the SR570 view changed.
//	void sr570ViewModeChanged(CLSSR570View::ViewMode);

public slots:
	/// Convenience method that sets the output view mode for the channel.
	void setOutputViewMode(CLSSIS3820ScalerChannelView::OutputViewMode mode);
//	/// Convenience method that sets the view mode of the SR570.
//	void setSR570ViewMode(CLSSR570View::ViewMode mode);
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

protected slots:
	/// Handles building the appropriate string when the reading changed.
	void onReadingChanged();
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
//	CLSSR570View *sr570View_;
    AMCurrentAmplifierView *sr570View_;
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

protected slots:
	/// Handles starting the scaler.
	void startScanning();
	/// Hanldes stopping the scaler.
	void stopScanning();
	/// Handles setting the mode.
	void setContinuous(int index);
	/// Handles updating the combo box if the mode was changed from elsewhere.
	void onContinuousChanged(bool isContinuous);
	/// Handles changing the icon when the status changes.
	void onStatusChanged(bool status);
	/// Handles setting the time.
	void setTime();
	/// Handles setting the spin box when the time changes on the scaler.
	void onTimeChanged(double time);
	/// Handles setting the number of scans per buffer.
	void setScansPerBuffer();
	/// Handles setting the number of total scans.
	void setTotalNumberOfScans();

//	/// Handles switching all the SR570 views to match the most recent change.
//	void onSR570ViewChanged(CLSSR570View::ViewMode mode);
	/// Handles switching all the output views to match the most recent change.
	void onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode);

protected:
	/// Pointer to the scalar being viewed.
	CLSSIS3820Scaler *scaler_;

	/// Button that handles starting the scanning mode of the scaler.
	QPushButton *startButton_;
	/// Button that handles stopping the scanning mode of the scaler.
	QPushButton *stopButton_;
	/// Button that handles setting the mode of the scaler.
	QComboBox *modeChoice_;
	/// Label holding the overal scanning status of the scaler.  Matches the scanning button.
	QLabel *status_;
	/// Spin box holding the amount of time to per point.
	QSpinBox *time_;
	/// Spin box holding the number of scans per buffer.
	QSpinBox *scansPerBuffer_;
	/// Spin box holding the total number of scans.
	QSpinBox *totalScans_;
	/// Holds all of the individual channel views.
	QList<CLSSIS3820ScalerChannelView *> channelViews_;

	/// A layout for all of the channel views
	QVBoxLayout *channelLayout_;
	/// The main layout for the widget
	QVBoxLayout *mainVL_;
};

#endif // CLSSIS3820SCALERVIEW_H
