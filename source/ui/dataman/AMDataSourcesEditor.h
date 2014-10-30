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


#ifndef AMDATASOURCESEDITOR_H
#define AMDATASOURCESEDITOR_H

#include <QWidget>
#include <QMenu>
#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>
#include "dataman/AMScanSetModel.h"


/// This widget is used inside AMGenericScanEditor to let users create, delete, and modify the analysis chains / analysis blocks for a set of scans.  It's a quick first prototype, the eventual interface should let users create custom analysis chains and edit the parameters at each level within the chain.  This version only displays the current analysis blocks.
class AMDataSourcesEditor : public QWidget
{
Q_OBJECT
public:
 	virtual ~AMDataSourcesEditor();
	explicit AMDataSourcesEditor(AMScanSetModel* model, QWidget *parent = 0);


signals:

public slots:
	/// Call this to set the current scan that we edit data sources for
	void setCurrentScan(AMScan* scan);
	/// Call this to set the current scan that we edit data sources for. (The index refers to the scan index in our AMScanSetModel).
	/*! This is an overloaded function.*/
	void setCurrentScan(int scanIndex);

protected slots:
	/// Called when the currently-selected scan or data source (in our list view) is changed
	void onSetViewIndexChanged(const QModelIndex& selected, const QModelIndex& deselected);
	/// Called when a user clicks the 'close' button on a data source.
	void onCloseButtonClicked(const QModelIndex& index);

	/// Called when the 'add data source' button is clicked
	void onAddDataSourceButtonClicked();

	/// Called when the user comes up with a name for their new data source
	void onNewDataSourceNamed();

	/// Called when the user finishes editing a data source description
	void descriptionEditingFinished();
	/// Called when the current data source description is changed on the model side.
	void onDataSourceDescriptionChanged();

	/// Called when the custom context menu is requested.
	void onCustomContextMenuRequested(QPoint point);

protected:
	/// Shows all data sources for the current scan regardless of whether or not they are hidden from users.
	void showAllDataSources(bool showAll);

	/// This is a model of scans and their data sources. Inside an AMGenericScanEditor, we share this between ourselves, the main editor, and the AMScanView.
	AMScanSetModel* model_;

	/// Helper function: returns the currently selected scan index.  If a data source is selected, this is the index of its parent scan. If a scan is selected, this is the index of that scan.  If nothing is selected, returns -1
	int currentScanIndex() const;
	/// Helper function: returns the currently selected data source index.  If a scan (or nothing) is currently selected, returns -1
	int currentDataSourceIndex() const;

	/// Helper function: removes and deletes any current 'detail editors' (data source-specific parameter editors)
	void removeDetailEditor() {
		if(detailEditor_) {
			detailEditor_->deleteLater();
			detailEditor_ = 0;
		}
	}

	/// Helper function: install a 'detail editor' (data source-specific parameter editor). If there is an existing detail editor, it will be removed first.  The \c newDetailEditor should be initialized, and already connected to the data source it's intended to edit.  Alternatively, if \c newDetailEditor = 0, the widget is left blank.
	/*! \todo Future performance optimization: If the existing detail editor is for the same type of data source as the new one, devise a way to re-use the existing widget, rather than deleting and re-creating.)*/
	void installDetailEditor(QWidget* newDetailEditor) {
		if(detailEditor_)
			removeDetailEditor();

		detailEditor_ = newDetailEditor;

		if(detailEditor_)
			detailEditorLayout_->addWidget(detailEditor_);
	}


	/// Flag to indicate that we're currently editing the name of a new (not-yet created) data source.
	bool editingNewDataSourceName_;

	/// Name of the type of analysis block will be added.
	QString nameOfAnalysisBlockToBeAdded_;

	/// A data source-specific editor widget, to edit the unique parameters of a data source.  Returned by AMDataSource::createDetailEditor().
	QWidget* detailEditor_;

	/// Flag holding whether all of the data sources should be visible or not, despite their state of hidden from users.
	bool showAllDataSources_;

	QVBoxLayout *verticalLayout;
	QTreeView *scanSetView_;
	QHBoxLayout *addAnalysisToolButtonLayout_;
	QToolButton *addDataSourceButton_;
	QSpacerItem *horizontalSpacer_2;
	QGridLayout *analysisDetailsLayout_;
	QLineEdit *nameEdit_;
	QLabel *descriptionLabel_;
	QLineEdit *descriptionEdit_;
	QLabel *nameLabel_;
	QFrame *lineBreak_;
	QVBoxLayout *detailEditorLayout_;
};

#endif // AMDATASOURCESEDITOR_H
