#include "AMItemModel.h"
#include "dataman/AMRunExperimentItems.h"

#include <QDebug>

AMItemModel::AMItemModel(QObject *parent) :
		QStandardItemModel(parent)
{
}

bool AMItemModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {

	QStandardItem* item = this->itemFromIndex(parent);
	if(!item)
		return QStandardItemModel::dropMimeData(data, action, row, column, parent);

	if(row >= 0 && column >= 0)
		item = item->child(row, column);

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
