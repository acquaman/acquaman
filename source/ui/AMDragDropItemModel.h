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


#ifndef AMITEMMODEL_H
#define AMITEMMODEL_H

#include "acquaman.h"
#include <QStandardItemModel>
#include <QStandardItem>

/// This extension of QStandardItem allows user-type items (ex: AMRunModelItem, AMExperimentModelItem) to define their own behaviour in response to drag and drop events.  Each individual item can handle or reject a drop by re-implementing dropMimeData(const QMimeData *data, Qt::DropAction action).  The base-class implementation rejects all drops.
class AMDragDropItem : public QStandardItem {
public:
	/// Constructor
 	virtual ~AMDragDropItem();
	AMDragDropItem(const QString& text = QString());

	/// Reimplement this function to handle or reject a drop action. Return false to reject, or true to accept.
	virtual bool dropMimeData(const QMimeData * data, Qt::DropAction action) { Q_UNUSED(data); Q_UNUSED(action); return false; }
	/// Re-implemented from QStandardItem to distinguish this item type. Subclasses should NOT re-implement this function.  If they do, changes need to be made to AMItemModel::dropMimeData() to handle the new type specifically.
	virtual int type() const { return AM::DragDropItem; }
};

/// This extension of QStandardItemModel allows user-type items (ex: AMRunModelItem, AMExperimentModelItem) to define their own behaviour in response to drag and drop events.
class AMDragDropItemModel : public QStandardItemModel
{
Q_OBJECT
public:
 	virtual ~AMDragDropItemModel();
	explicit AMDragDropItemModel(QObject *parent = 0);

	/// Re-implemented to check if the item supports this drop, and if it does, passes the drop along to the item.
	virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);


	/// Re-implemented to indicate that we potentially accept all drop actions. It will be up to the items to decide in dropMimeData if they actually accept the drop.
	virtual Qt::DropActions supportedDropActions() const {
		return (Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);
	}

	/// Re-implemented to add the text/uri-list MIME type to the list of mime-types that are accepted for drops
	virtual QStringList mimeTypes() const {

		QStringList rv = QStandardItemModel::mimeTypes();
		rv << "text/uri-list";

		return rv;
	}

signals:

public slots:

protected:

};

#endif // AMITEMMODEL_H
