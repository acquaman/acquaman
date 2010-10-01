#ifndef AMSCANEDITORMODELITEM_H
#define AMSCANEDITORMODELITEM_H

#include "ui/AMDragDropItemModel.h"

class AMGenericScanEditor;

/// This class provides a QStandardItem, representing a scan editor window, that is suitable for use in the AMWindowPaneModel.  It implements Drag-and-Drop capability so that any drop item containing a list of scans will be opened by the scan editor window.  The drop item mime data is in the form of a uri-list containing "amd://databaseConnectionName/tableName/objectId" protocol URLs, as documented in AMGenericScanEditor::dropEvent().
class AMScanEditorModelItem : public AMDragDropItem
{
public:

	/// Constructor takes a pointer to a valid scan editor \c editorWidget, and an optional \c iconFileName
	/*! This constructor will set the AM::WidgetRole to point this item at the editor widget. It will also set the AM::CanCloseRole to true, so that this editor can be closed from the window pane manager sidebar. (For more information, see AMWindowPaneModel.) Finally, it will set the item flags to enable drop capability.*/
	AMScanEditorModelItem(AMGenericScanEditor* editorWidget, const QString& iconFileName = QString());

	/// This function accepts Drag-and-Drop mime data when it is in the form of a uri-list containing "amd://databaseConnectionName/tableName/objectId" protocol URLs, as documented in AMGenericScanEditor::dropEvent().  Returns false to reject, or true to accept.
	virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action);


};

#endif // AMSCANEDITORMODELITEM_H
