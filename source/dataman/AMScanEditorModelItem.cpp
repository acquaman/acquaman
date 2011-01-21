/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
