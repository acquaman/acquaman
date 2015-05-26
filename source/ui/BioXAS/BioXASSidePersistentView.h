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


#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QInputDialog>

#include "ui/BioXAS/BioXASBeamlineBeamView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorConfigurationView.h"

class BioXASSidePersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSidePersistentView(QWidget *parent = 0);
	/// Destructor.
    virtual ~BioXASSidePersistentView();

protected slots:
	/// Handles updating the view when the scaler connection state changes.
	void onScalerConnectedChanged();

protected:
	/// Editor for the mono's energy.
	AMExtendedControlEditor *energyControlEditor_;
	/// Editor for the mono's region.
	BioXASSSRLMonochromatorRegionControlEditor *regionControlEditor_;
	/// Editor for the mono's bragg motor angle.
	AMExtendedControlEditor *braggControlEditor_;
	/// Scaler channel views for i0, iT, and i2 channels.
	QGroupBox *channelViews_;
};

#endif // BIOXASSIDEPERSISTENTVIEW_H
