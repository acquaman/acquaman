#include "AMItemModel.h"

#include "dataman/AMRunExperimentItems.h"

AMItemModel::AMItemModel(QObject *parent) :
		QStandardItemModel(parent)
{
}

bool AMItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {

	QStandardItem* parentItem = this->itemFromIndex(parent);
	if(!parentItem)
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);

	QStandardItem* item = parentItem->child(row, column);
	if(!item)
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);

	switch(item->type()) {

		// case AM::RunItem:
		//	break;

	case AM::ExperimentItem: {
			AMExperimentModelItem* ei = static_cast<AMExperimentModelItem*>(item);
			return ei->dropMimeData(data, action);
		}
		break;

	default:
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);
	}
}
