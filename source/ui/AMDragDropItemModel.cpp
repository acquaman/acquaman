#include "AMDragDropItemModel.h"
#include "dataman/AMRunExperimentItems.h"

#include <QDebug>


AMDragDropItem::AMDragDropItem(const QString& text) : QStandardItem(text) {
	setDropEnabled(true);
}


AMDragDropItemModel::AMDragDropItemModel(QObject *parent) :
		QStandardItemModel(parent)
{
}

bool AMDragDropItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {

	QStandardItem* item = this->itemFromIndex(parent);
	if(!item)
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);

	if(row >= 0 && column >= 0)
		item = item->child(row, column);

	if(!item)
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);

	switch(item->type()) {

	case AM::DragDropItem: {
			AMDragDropItem* ddi = static_cast<AMDragDropItem*>(item);
			return ddi->dropMimeData(data, action);
		}
		break;

	default:
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);
	}
}
