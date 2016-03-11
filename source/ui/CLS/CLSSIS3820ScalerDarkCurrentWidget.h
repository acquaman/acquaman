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


#ifndef CLSSIS3820SCALERDARKCURRENTWIDGET_H
#define CLSSIS3820SCALERDARKCURRENTWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>

#include "beamline/CLS/CLSSIS3820Scaler.h"

/// This is a class that allows a user to manually instigate the chain of events leading to a dark current correction measurement.
class CLSSIS3820ScalerDarkCurrentWidget : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSSIS3820ScalerDarkCurrentWidget(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentWidget();

	/// Returns the scaler being viewed.
	CLSSIS3820Scaler* scaler() const { return scaler_; }

signals:
	/// Emitted when the scaler being viewed has changed.
	void scalerChanged(CLSSIS3820Scaler *newScaler);

public slots:
	/// Refreshes the view.
	void refresh();
    /// Sets the scaler being viewed.
    void setScaler(CLSSIS3820Scaler *newScaler);

protected slots:
	/// Updates the time box.
	void updateTimeBox();
	/// Updates the collect button.
	void updateCollectButton();

	/// Handles initiating a scaler dark current measurement, provided the time entered is greater than zero.
    void onCollectButtonClicked();

protected:
    /// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;
	/// The time entry box.
	QSpinBox* timeBox_;
	/// The collect button.
	QPushButton* collectButton_;
};

#endif // CLSDARKCURRENTWIDGET_H
