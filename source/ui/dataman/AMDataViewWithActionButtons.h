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


#ifndef AMDATAVIEWWITHACTIONBUTTONS_H
#define AMDATAVIEWWITHACTIONBUTTONS_H

#include "ui/dataman/AMDataView.h"
#include "ui_AMDataViewActionsBar.h"

/// This class adds buttons for editing, comparing, and exporting the selected scans in an AMDataView.
class AMDataViewWithActionButtons : public QWidget
{
	Q_OBJECT
public:
	/// Constructs an AMDataView inside a widget that also provides buttons for the user to edit, compare, and export scans.
	explicit AMDataViewWithActionButtons(AMDatabase* database = AMDatabase::database("user"), QWidget *parent = 0);

	virtual ~AMDataViewWithActionButtons();

	/// Access the AMDataView contained inside this widget
	AMDataView* dataView() const { return dataView_; }

	/// Must be called after the constructor to create the correct views and connections by calling the virtual createDataView()
	virtual void buildView();

signals:
	/// Emitted when the user attempts to open the selected scans
	void selectionActivated(const QList<QUrl>&);
	/// Emitted when the user attempts to open the selected scans in separate windows
	void selectionActivatedSeparateWindows(const QList<QUrl> &);
	/// Emitted when the user attempts to export the selected scans. (This action is (hopefully) completed elsewhere, so that we don't couple the AMDataView to the export system)
	void selectionExported(const QList<QUrl>&);
	/// Emitted when the user wants to open scan configurations of the selected scans from the database.
	void launchScanConfigurationsFromDb(const QList<QUrl> &);
	/// Emitted when the user wants to fix a scan that uses CDF files.
	void fixCDF(const QUrl &);

public slots:

protected slots:
	/// When the "open in same window" action happens
	void onCompareScansAction() {
		emit selectionActivated(dataView_->selectedItems());
	}

	/// When the "open in separate window" action
	void onEditScansAction() {
		emit selectionActivatedSeparateWindows(dataView_->selectedItems());
	}

	/// When the "export scans" action happens
	void onExportScansAction() {
		emit selectionExported(dataView_->selectedItems());
	}

	/// When anything is double-clicked.  If more than 0 items are selected, we emit selectionActivated().
	void onDoubleClick();

	/// When the base class's selection changes. We might need to enable or disable some of the buttons.
	void onSelectionChanged();

	/// When the user chooses to launch scan configurations from the database.  If more than 0 items are selected, we emit launchScanConfigurationsFromDb().
	void onLaunchScanConfigurationsFromDb();

	/// When the user chooses to fix a CDF file, this finds the QUrl associated with that scan and then passes it on.
	void onFixCDF();

protected:
	/// Called to instantiate dataview. If you wish to subclass AMDataView, you can instantiate your subclassed version by reimplementing this function.
	virtual AMDataView* createDataView(AMDatabase *database);

protected:
	Ui::AMDataViewActionsBar* ui_;
	AMDataView* dataView_;

	AMDatabase *database_;
};

#endif // AMDATAVIEWWITHACTIONBUTTONS_H
