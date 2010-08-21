#ifndef AMITEMMODEL_H
#define AMITEMMODEL_H

#include "acquaman.h"
#include <QStandardItemModel>

/// This extension of QStandardItemModel allows user-type items (ex: AMRunModelItem, AMExperimentModelItem) to define their own behaviour in response to drag and drop events.
class AMItemModel : public QStandardItemModel
{
Q_OBJECT
public:
	explicit AMItemModel(QObject *parent = 0);

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
