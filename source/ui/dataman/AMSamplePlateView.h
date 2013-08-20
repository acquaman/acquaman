#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QWidget>
#include <QListView>
#include <QAbstractListModel>
#include <QStyledItemDelegate>

#include "dataman/AMSamplePlate.h"

class AMSamplePlateItemModel : public QAbstractListModel
{
Q_OBJECT
public:
	AMSamplePlateItemModel(AMSamplePlate *samplePlate, QObject *parent = 0);

	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected slots:
	/// Received from AMSamplePlate. Used to implement beginInsertRows.
	void onSampleAboutToBeAdded(int index);

	/// Received from AMSamplePlate. Used to implement endInsertRows.
	void onSampleAdded(int index);

	/// Received from AMSamplePlate. Used to implement beginRemoveRows.
	void onSampleAboutToBeRemoved(int index);

	/// Received from AMSamplePlate. Used to implement endRemoveRows.
	void onSampleRemoved(int index);

	/// Received fomr AMSamplePlate. Used to implement dataChanged().
	void onSampleChanged(int index);

protected:
	AMSamplePlate *samplePlate_;
};

class AMSamplePlateItemDelegate : public QStyledItemDelegate{
Q_OBJECT
public:
	AMSamplePlateItemDelegate(QObject *parent = 0);
	virtual ~AMSamplePlateItemDelegate();

	// Reimplemented delegate functions
	////////////////////////
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
	//QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// Don't do anything to set back the model data...
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	///////////////////////////

	/*
signals:
	/// Emitted when the "Mark" button is pressed on a specific row in the list
	void rowMarkPressed(int row);
	/// Emitted when the "Move To" button is pressed on a specific row in the list
	void rowMoveToPressed(int row);
	/// Emitted when the "Remove" button is pressed on a specific row in the list
	void rowRemovePressed(int row);

	/// Emitted when the custom context menu for "Additional Information" is activated
	void additionalInformationRequested(int row);

protected:
	/// disable the editor closing / committing?
	bool eventFilter(QObject *object, QEvent *event) {
		// qdebug() << "Event:" << event << "Type:" << event->type();

		if(event->type() == QEvent::FocusOut)
			return false;

		return QStyledItemDelegate::eventFilter(object, event);
	}
	*/
};

class AMSamplePlateView : public QWidget
{
Q_OBJECT
public:
	AMSamplePlateView(AMSamplePlate *samplePlate, QWidget *parent = 0);

protected:
	AMSamplePlate *samplePlate_;
	AMSamplePlateItemModel *samplePlateItemModel_;
	QListView *sampleListView_;
};

#endif // AMSAMPLEPLATEVIEW_H
