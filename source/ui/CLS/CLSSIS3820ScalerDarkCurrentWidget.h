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
#include "actions3/AMActionRunner3.h"

#define DARK_CURRENT_DWELL_TIME_MIN 0
#define DARK_CURRENT_DWELL_TIME_MAX 10000

#define MILLISECONDS_PER_SECOND 1000

/// This is a class that allows a user to manually instigate the chain of events leading to a dark current correction measurement.
class CLSSIS3820ScalerDarkCurrentWidget : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit CLSSIS3820ScalerDarkCurrentWidget(CLSSIS3820Scaler *scaler, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSSIS3820ScalerDarkCurrentWidget();

signals:
	/// Emitted when the scaler being viewed has changed.
	void scalerChanged(CLSSIS3820Scaler *newScaler);

public slots:
    /// Sets the scaler being viewed.
    void setScaler(CLSSIS3820Scaler *newScaler);

protected slots:
    /// Handles setting the collect button as 'Enabled' when the scaler is not scanning, and 'Disabled' when it is.
    void onScalerScanningChanged();
    /// Handles emitting collectButtonClicked() signal when the collect button is clicked, with the dwell time entered argument. Signal is emitted only if the time entered is greater than zero.
    void onCollectButtonClicked();

protected:
    /// The scaler being viewed.
	CLSSIS3820Scaler *scaler_;
    /// The entry widget for updating the desired dwell time.
    QSpinBox* timeEntry_;
    /// Pressing this button should start the process of taking a dark current measurement.
    QPushButton* collectButton_;

};

#endif // CLSDARKCURRENTWIDGET_H
