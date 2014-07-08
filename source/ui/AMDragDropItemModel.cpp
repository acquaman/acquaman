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


#include "AMDragDropItemModel.h"
#include "dataman/AMRunExperimentItems.h"

 AMDragDropItem::~AMDragDropItem(){}
AMDragDropItem::AMDragDropItem(const QString& text) : QStandardItem(text) {
	setDropEnabled(true);
}


 AMDragDropItemModel::~AMDragDropItemModel(){}
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
