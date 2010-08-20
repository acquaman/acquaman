#include "AMRunExperimentItems.h"
#include "dataman/AMDatabase.h"

#include <QDebug>

QVariant AMRunModelItem::data(int role) const {
	if(role == Qt::DisplayRole)
		return QStandardItem::data(Qt::EditRole).toString() + QStandardItem::data(AM::DateTimeRole).toDateTime().toString(", MMM d (yyyy)");
	else
		return QStandardItem::data(role);
}

void AMRunModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), "Runs", "name", value);
	}
	QStandardItem::setData(value, role);
}

void AMExperimentModelItem::setData(const QVariant &value, int role) {
	if(role == Qt::EditRole) {
		db_->update( data(AM::IdRole).toInt(), "Experiments", "name", value);
	}
	QStandardItem::setData(value, role);
}

bool AMExperimentModelItem::dropMimeData(const QMimeData *data, Qt::DropAction action) {
	qDebug() << "Drop happened!";
	return false;
	/// \todo
}
