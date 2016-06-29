
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

#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/PGM/PGMBeamline.h"

class AMExtendedControlEditor;
class PGMBeamStatusView;
class PGMBPMStatusView;

/// The persistent panel that lives on the right side of the application.
class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:
	/// Notifier that the selected control in the beam status buttons view has changed.
	void beamlineStatusControlClicked(AMControl *control);

public slots:

protected slots:

protected:
	/// create and layout the content of the persistent component
	QLayout * createPersistentLayout();


signals:

public slots:

protected:
	/// Contains beamline current lifetime information
	PGMBeamStatusView *beamStatusView_;
	/// Contains the beam position monitor information
	PGMBPMStatusView *bpmStatusView_;

	/// Group box editor with extended controls.
	AMExtendedControlEditor *energyControlEditor_;
	/// Editor for the grating selection.
	AMExtendedControlEditor *gratingControlEditor_;
};

#endif // PGMPERSISTENTVIEW_H
