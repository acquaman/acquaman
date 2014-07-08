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

#ifndef AMPERIODICTABLEDIALOG_H
#define AMPERIODICTABLEDIALOG_H

#include <QDialog>

#include "util/AMElement.h"

/*! This class encapsulates the AMPeriodicTableView inside a QDialog.  The purpose is to create a modal dialog that passes the element that was clicked
	to the calling class.  Essentially, by clicking on one of the element tool buttons, that calls the accept() signal and passes that element.
*/
class AMPeriodicTableDialog : public QDialog
{
	Q_OBJECT
public:
	/// Constructor.  Builds the dialog to look exactly like AMPeriodicTableView.
 	virtual ~AMPeriodicTableDialog();
	explicit AMPeriodicTableDialog(QWidget *parent = 0);

	/// Static member.  Builds a dialog and returns an AMElement *.  If the dialog is cancelled then the dialog returns 0.
	static AMElement *getElement(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the element passing from the periodic table view.
	void onElementSelected(AMElement *el);

protected:
	/// Returns the current state of element pointed to by this dialog.
	AMElement *element() const { return element_; }

	/// Pointer to the element this dialog is meant to provide.  Defaults to 0.
	AMElement *element_;
};

#endif // AMPERIODICTABLEDIALOG_H
