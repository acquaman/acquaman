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


#ifndef AMSELECTIONDIALOG_H
#define AMSELECTIONDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QStringList>

/// This class provides a means of showing a selection of items and then providing a means of retrieving them afterwards.
class AMSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the dialog from the provided string list.
 	virtual ~AMSelectionDialog();
	AMSelectionDialog(const QString &title, const QStringList &items, QWidget *parent = 0);

	/// Returns the items that are selected as a string list.
	QStringList selectedItems() const;

	/// allows to select nothing
	void enableEmptySelectedItemsAllowed();

protected slots:
	void onItemCheckStateChanged(int state);

protected:
	/// Flag to mark whether allows select nothing (default False)
	bool emptySelectedItemsAllowed_;

	/// The button group that holds all of the check boxes.
	QButtonGroup *items_;

	/// The okeyButton, which is controlled by emptySelectedItemsAllowed_
	QPushButton *okayButton_;
};

#endif // AMSELECTIONDIALOG_H
