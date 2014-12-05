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


#include "AMScanEditorModelItem.h"

#include <QUrl>
#include <QList>

#include "ui/AMWindowPaneModel.h"
#include "application/AMDatamanAppController.h"
#include "ui/dataman/AMGenericScanEditor.h"
#include "dataman/AMScan.h"

AMScanEditorModelItem::AMScanEditorModelItem(AMGenericScanEditor *editorWidget, AMDatamanAppController* controller)
	: AMDragDropItem()
{
	appController_ = controller;

	icons_.insert("default", QIcon(":/applications-science.png"));
	icons_.insert("succeeded", QIcon(":/22x22/greenCheck.png"));
	icons_.insert("cancelled", QIcon(":/22x22/orangeX.png"));
	icons_.insert("failed", QIcon(":/22x22/redCrash.png"));
	icons_.insert("running", QIcon(":/22x22/dialog-question.png"));

	backgrounds_.insert("default", this->background());
	backgrounds_.insert("succeeded", QBrush(QColor(126, 255, 106)));
	backgrounds_.insert("cancelled", QBrush(QColor(255, 176, 106)));
	backgrounds_.insert("failed", QBrush(QColor(255, 104, 106)));
	backgrounds_.insert("running", QBrush(QColor(100, 149, 237)));

	setData(qVariantFromValue(qobject_cast<QWidget*>(editorWidget)), AM::WidgetRole);
	setData(true, AM::CanCloseRole);
	setData(true, AMWindowPaneModel::DockStateRole);

	editorWidget->setWindowIcon(QIcon(":/applications-science.png"));

	setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
	setDropEnabled(true);
}

AMScanEditorModelItem::~AMScanEditorModelItem(){}

QVariant AMScanEditorModelItem::data(int role) const{
	switch(role){
	case AMScanEditorModelItem::IsModifiedRole:
		if(editorWidget())
			return !editorWidget()->canCloseEditor(false);
		return false;
	case AMScanEditorModelItem::IsMultipleRole:
		if(editorWidget() && editorWidget()->scanCount() > 1)
			return true;
		return false;
	case AMScanEditorModelItem::ScanTextRole:
		if(editorWidget()){
			QString retVal;
			if(editorWidget()->scanCount() == 0)
				retVal = "Empty Editor";
			else if(editorWidget()->scanCount() == 1)
				retVal.append(QString("%1 [%2]").arg(editorWidget()->scanAt(0)->name()).arg(editorWidget()->scanAt(0)->id()));
			else{
				retVal.append(QString("%1 Scans [%2").arg(editorWidget()->scanCount()).arg(editorWidget()->scanAt(0)->id()));
				for(int x = 1, size = editorWidget()->scanCount(); x < size; x++)
					retVal.append(QString(",%1").arg(editorWidget()->scanAt(x)->id()));
				retVal.append("]");
			}

			if(!editorWidget()->canCloseEditor(false))
				retVal.append(" (Modified)");
			return retVal;
		}
		return QString();
	default:
		return AMDragDropItem::data(role);
	}
}


bool AMScanEditorModelItem::dropMimeData(const QMimeData *mimeData, Qt::DropAction action) {

	if(action != Qt::CopyAction)
		return false;

	if(!mimeData->hasUrls())
		return false;

	AMGenericScanEditor* editor = editorWidget();

	if(!editor || !appController_)
		return false;

	return appController_->dropScanURLs(mimeData->urls(), editor);

}

AMGenericScanEditor *AMScanEditorModelItem::editorWidget() const
{
	return qobject_cast<AMGenericScanEditor*>(data(AM::WidgetRole).value<QWidget*>());
}

void AMScanEditorModelItem::editorWasClicked()
{
	setBackground(backgrounds_.value("default"));
}

void AMScanEditorModelItem::scanActionStateChanged(const QString &state, bool isSelected)
{
	editorWidget()->setWindowIcon(icons_.value(state, icons_.value("default")));

	if (!isSelected)
		setBackground(backgrounds_.value(state, backgrounds_.value("default")));
}
