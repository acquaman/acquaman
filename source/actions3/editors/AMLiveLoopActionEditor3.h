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


#ifndef AMLIVELOOPACTIONEDITOR3_H
#define AMLIVELOOPACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMLoopAction3.h"

class QSpinBox;
class QLabel;

/// This class implements an editor of a live AMLoopAction.  It is different from the normal editor because it modifies the action as it's running.  This is distinct difference in paradigm.
class AMLiveLoopActionEditor3 : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  Accepts an AMLoopAction (NOT info!).  If modifying actions becomes more necessary, this will need to be made Q_INVOKABLE.
 	virtual ~AMLiveLoopActionEditor3();
	AMLiveLoopActionEditor3(AMLoopAction3 *action, QWidget *parent = 0);

protected slots:
	/// When the action loop count updates.  Update the spin box accordingly.
	void onLoopCountChanged();
	/// Sets the new loop count.
	void setLoopCount();
	/// Updates the current iteration label when the iteration changes.
	void onIterationUpdate(int val);

protected:
	/// The action pointer.
	AMLoopAction3 *action_;
	/// The spin box holding the maximum loop count.
	QSpinBox *loopCount_;
	/// The label holding the current iteration of the loop.
	QLabel *iteration_;
};

#endif // AMLIVELOOPACTIONEDITOR3_H
