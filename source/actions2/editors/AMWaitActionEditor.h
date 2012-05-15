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


#ifndef AMWAITACTIONEDITOR_H
#define AMWAITACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/AMWaitActionInfo.h"

class QDoubleSpinBox;

/// This class implements a simple editor widget for AMWaitActionInfo that can be shown inside action tree views.  It provides a spin box to set the number of seconds to wait.
class AMWaitActionEditor : public QFrame
{
	Q_OBJECT
public:
	/// Constructor: Must be Q_INVOKABLE, and accept an AMWaitActionInfo as argument
	Q_INVOKABLE AMWaitActionEditor(AMWaitActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When editing is finished, calls info_->setSecondsToWait() with the value in the spin box.
	void onSpinBoxEditingFinished();

protected:
	QDoubleSpinBox* spinBox_;
	AMWaitActionInfo* info_;

};

#endif // AMWAITACTIONEDITOR_H
