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


#ifndef AMLOOPACTIONEDITOR3_H
#define AMLOOPACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMLoopActionInfo3.h"

class QSpinBox;

/// This class implements a simple editor widget for AMLoopActionInfo that can be shown inside action tree views.  It provides a spin box to set the number of iterations to run.
class AMLoopActionEditor3 : public QFrame
{
	Q_OBJECT
public:
	/// Constructor: Must be Q_INVOKABLE, and accept an AMLoopActionInfo as argument
 	virtual ~AMLoopActionEditor3();
	Q_INVOKABLE AMLoopActionEditor3(AMLoopActionInfo3* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When editing is finished, calls info_->setLoopCount() with the value in the spin box.
	void onSpinBoxEditingFinished();
	/// When the info changes, we need to update the view here.
	void onLoopCountChanged(int val);

protected:
	QSpinBox* spinBox_;
	AMLoopActionInfo3* info_;

};

#endif // AMLOOPACTIONEDITOR_H
