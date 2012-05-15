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

class CLSSIS3820Scaler;
class CLSSIS3820ScalerChannel;

/*!
  This class builds a view around a CLSSIS3820ScalerChannel.  It provides a simple view that has a check box
  and the current reading.
  */

class CLSSIS3820ScalerChannelView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a CLSSIS3820ScalarChannel and builds a view around it.
	CLSSIS3820ScalerChannelView(CLSSIS3820ScalerChannel *channel, QWidget *parent = 0);

protected slots:
	/// Handles building the appropriate string when the reading changed.
	void onReadingChanged(int reading);

protected:
	/// Pointer to the channel being viewed.
	CLSSIS3820ScalerChannel *channel_;

	/// Label holding the current reading of the scaler channel.
	QLabel *readingLabel_;
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

protected slots:
	/// Handles setting the scanning mode.
	void setScanning(bool scanning);
	/// Handles setting the mode.
	void setContinuous(bool isContinuous);
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

protected:
	/// Pointer to the scalar being viewed.
	CLSSIS3820Scaler *scaler_;

	/// Button that handles setting the scanning mode of the scaler.
	QPushButton *scanningButton_;
	/// Button that handles setting the mode of the scaler.
	QPushButton *modeButton_;
	/// Label holding the overal scanning status of the scaler.  Matches the scanning button.
	QLabel *status_;
	/// Spin box holding the amount of time to per point.
	QSpinBox *time_;
	/// Spin box holding the number of scans per buffer.
	QSpinBox *scansPerBuffer_;
	/// Spin box holding the total number of scans.
	QSpinBox *totalScans_;
};

#endif // CLSSIS3820SCALERVIEW_H
