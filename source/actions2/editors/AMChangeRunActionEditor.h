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


#ifndef AMCHANGERUNACTIONEDITOR_H
#define AMCHANGERUNACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/AMChangeRunActionInfo.h"

class AMRunSelector;
class QLineEdit;

/// Provides in-place editor for an action to change the current run.
class AMChangeRunActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor must accept a AMChangeRunActionInfo as argument
	Q_INVOKABLE AMChangeRunActionEditor(AMChangeRunActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the radio button is toggled to switch between existing run / new run.
	void onNewRunRadioButtonToggled(bool newRun);
	/// Called when the current run is changed in the selector
	void onExistingRunChanged(int);
	/// Called when the name of the new run is changed
	void onNewRunNameChanged(const QString& newRunName);

protected:
	AMRunSelector* runSelector_;
	QLineEdit* newNameEdit_;
	AMChangeRunActionInfo* info_;

};

#endif // AMCHANGERUNACTIONEDITOR_H
