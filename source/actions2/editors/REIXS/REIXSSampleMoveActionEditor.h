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


#ifndef REIXSSAMPLEMOVEACTIONEDITOR_H
#define REIXSSAMPLEMOVEACTIONEDITOR_H

#include <QFrame>

class QDoubleSpinBox;
class QComboBox;
class QRadioButton;

#include "actions2/actions/REIXS/REIXSSampleMoveActionInfo.h"

/// This widget provides an in-place editor for changing samples / sample positions.
/*! It's a bit complicated because this action can now operate in two different modes: moving to a predetermined X,Y,Z,Theta positions, or moving to the position of a sample on a stored sample plate.  It's also a bit messy because we use the beamline's current sample plate (REIXSBeamline::samplePlate()) to fill the menu of available samples, but the REIXSSampleMoveActionInfo might be set up to operate on any sample plate in the database -- not necessarily the current. Not a great design; don't use this as an example. */
class REIXSSampleMoveActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor must be Q_INVOKABLE and accept a REIXSSampleMoveActionInfo*.
	Q_INVOKABLE REIXSSampleMoveActionEditor(REIXSSampleMoveActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the QComboBox entry is changed. Sets the sampleMoveInfo to use that sample index, but also copies the positions into the spin boxes.
	void onSampleActivated(int index);

	/// Called when the radio buttons are toggled. \c useSample will be true if the sampleMoveInfo should be set to use the stored sample plate; otherwise it is set to use the fixed positions.
	void onFromSampleToggled(bool useSample);

	/// Called when editing of the spin boxes is finished.
	void onSpinBoxEditingFinished();

protected:
	QComboBox* sample_;
	QDoubleSpinBox* x_, *y_, * z_, *theta_;
	QRadioButton* fromSample_, *fromPositions_;

	REIXSSampleMoveActionInfo* info_;

};

#endif // REIXSSAMPLEMOVEACTIONEDITOR_H
