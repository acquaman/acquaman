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


#ifndef AMSTARTSCREEN_H
#define AMSTARTSCREEN_H

#include <QDialog>
class AMRunSelector;

/// Provides a dialog (suitable for showing on startup) that allows users to choose the current run, or make a new one.  It saves the chosen run in AMUser::setCurrentRun(), and deletes itself when closed.
class AMStartScreen : public QDialog
{
	Q_OBJECT
public:
	/// Default constructor. To prevent the dialog from being closed until the user accepts a choice, set \c mustAccept to true.
 	virtual ~AMStartScreen();
	AMStartScreen(bool mustAccept = false, QWidget *parent = 0);

signals:

public slots:	
	/// When the dialog is accepted, we store the current run in AMUser::setCurrentRun().
	void accept();
	/// When the dialog is rejected, if \c mustAccept_ is true, we simply show it again. A choice must be accepted. (Unless the application is shutting down.)
	void reject();

protected:
	AMRunSelector *runSelector_;
	bool mustAccept_;
};




#endif // AMSTARTSCREEN_H
