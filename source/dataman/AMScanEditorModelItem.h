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


#ifndef AMSCANEDITORMODELITEM_H
#define AMSCANEDITORMODELITEM_H

#include "ui/AMDragDropItemModel.h"

class AMGenericScanEditor;
class AMDatamanAppController;

#include <QMap>

/// This class provides a QStandardItem, representing a scan editor window, that is suitable for use in the AMWindowPaneModel.  It implements Drag-and-Drop capability so that any drop item containing a list of scans will be sent to the Application Controller to be opened by that scan editor window.  The drop item mime data is in the form of a uri-list containing "amd://databaseConnectionName/tableName/objectId" protocol URLs, as documented in AMDatamanAppController::dropScanURLs().
class AMScanEditorModelItem : public AMDragDropItem
{
public:

	enum ScanEditorRoles { IsModifiedRole = AM::UserRole + 5909, IsMultipleRole, ScanTextRole };

	/// Constructor takes a pointer to a valid scan editor \c editorWidget, and an optional \c iconFileName
	/*! This constructor will set the AM::WidgetRole to point this item at the editor widget. It will also set the AM::CanCloseRole to true, so that this editor can be closed from the window pane manager sidebar. (For more information, see AMWindowPaneModel.) Finally, it will set the item flags to enable drop capability.*/
	AMScanEditorModelItem(AMGenericScanEditor* editorWidget, AMDatamanAppController* controller);

	virtual ~AMScanEditorModelItem();

	virtual QVariant data(int role) const;

	/// This function accepts Drag-and-Drop mime data when it is in the form of a uri-list containing "amd://databaseConnectionName/tableName/objectId" protocol URLs, as documented in AMGenericScanEditor::dropEvent().  Returns false to reject, or true to accept.
	virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action);

	/// Returns the editor widget.
	AMGenericScanEditor* editorWidget() const;

	/// Returns the app controller we relay drop events to.
	AMDatamanAppController* appController() const { return appController_; }

	/// Turns the background off highlighting when clicked.
	void editorWasClicked();
	/// Scan action state changed.  Updates the background if the item is not selected.  Acceptable state strings are default, succeeded, cancelled, failed, and running.
	void scanActionStateChanged(const QString &state, bool isSelected);

protected:
	AMDatamanAppController* appController_;

	/// A map between the state and it's icon.
	QMap<QString, QIcon> icons_;
	/// A map between the state and the background color.
	QMap<QString, QBrush> backgrounds_;
};

#endif // AMSCANEDITORMODELITEM_H
