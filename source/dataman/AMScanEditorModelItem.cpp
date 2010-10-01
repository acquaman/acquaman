#include "AMScanEditorModelItem.h"

#include "ui/AMWindowPaneModel.h"
#include "ui/AMGenericScanEditor.h"

#include <QUrl>
#include <QList>

AMScanEditorModelItem::AMScanEditorModelItem(AMGenericScanEditor *editorWidget, const QString &iconFileName) : AMDragDropItem()
{
	setData(qVariantFromValue(qobject_cast<QWidget*>(editorWidget)), AM::WidgetRole);
	setData(true, AM::CanCloseRole);
	setData(true, AMWindowPaneModel::DockStateRole);

	if(!iconFileName.isEmpty())
		editorWidget->setWindowIcon(QIcon(iconFileName));

	setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
	setDropEnabled(true);

}


bool AMScanEditorModelItem::dropMimeData(const QMimeData *mimeData, Qt::DropAction action) {

	if(action != Qt::CopyAction)
		return false;

	if(!mimeData->hasUrls())
		return false;

	AMGenericScanEditor* editor = qobject_cast<AMGenericScanEditor*>(data(AM::WidgetRole).value<QWidget*>());

	if(!editor)
		return false;

	return editor->dropScanURLs(mimeData->urls());

}
