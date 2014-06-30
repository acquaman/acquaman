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


#ifndef AMADDANALYSISBLOCKDIALOG_H
#define AMADDANALYSISBLOCKDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QPushButton;

/*!
  This is a simple dialog that provides a list of icons representing the available analysis blocks for the given dimensionality.

  \note Maybe at some point we should have analysis blocks go through the same registration system as actions?  Currently this will just keep need to be updated as new data sources are added.
  */
class AMAddAnalysisBlockDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds a dialog and populates it with the available analysis blocks for the given rank.
 	virtual ~AMAddAnalysisBlockDialog();
	AMAddAnalysisBlockDialog(int rank, QWidget *parent = 0);

	/// This method returns the name of a string based on the analysis block selected.  You must provide the rank so that the proper analysis blocks are populated.
	static QString getAnalysisBlock(int rank, QWidget *parent = 0);

	/// Returns teh current items name.
	QString name() const;

protected slots:
	/// Handles changing the dialog when the currently selected item changes.
	void onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

protected:
	/// Method that populates the dialog with 1D analysis blocks.
	void populateWith1D();
	/// Method that populates the dialog with 2D analysis blocks.
	void populateWith2D();

	/// Pointer to the list widget.
	QListWidget *listWidget_;
	/// The label holding the icon of the selected analysis block.
	QLabel *icon_;
	/// The label holding the name of the analysis block.
	QLabel *name_;
	/// The label holding the description of the analysis block.
	QLabel *description_;
	/// The add analysis block button.
	QPushButton *addAnalysisButton_;
};

#endif // AMADDANALYSISBLOCKDIALOG_H
