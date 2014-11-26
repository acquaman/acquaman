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


#ifndef AMSAMPLEPLATEVIEW_H
#define AMSAMPLEPLATEVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QAbstractListModel>
#include <QStyledItemDelegate>

#include "dataman/AMSamplePlate.h"

class QListView;
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QToolButton;
class QPushButton;

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

class AMSamplePlateItemEditor : public QWidget
{
Q_OBJECT
public:
	AMSamplePlateItemEditor(int row, QWidget *parent = 0);

signals:
	void rowMoveToPressed(int row);
	void rowMoreInfoPressed(int row);
	void rowClosedPressed(int row);

protected slots:
	void onMoveToButtonClicked();
	void onMoreInfoButtonClicked();
	void onCloseButtonPressed();

protected:
	int row_;

	QToolButton *moveToButton_;
	QToolButton *moreInfoButton_;
	QToolButton *closeButton_;
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
	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// Don't do anything to set back the model data...
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	///////////////////////////

signals:
	void rowMoveToPressed(int row);
	void rowMoreInfoPressed(int row);
	void rowClosedPressed(int row);
};

class AMSamplePlateView : public QGroupBox
{
Q_OBJECT
public:
	AMSamplePlateView(AMSamplePlate *samplePlate, QWidget *parent = 0);

public slots:
	void setSamplePlate(AMSamplePlate *samplePlate);

signals:
	void sampleAboutToBeRemoved(int index);


protected slots:
	void onRowMoveToPressed(int row);
	void onRowMoreInfoPressed(int row);
	void onRowClosedPressed(int row);

	void onSamplePlateModifiedChanged(bool isModified);
	void onSaveSamplePlateButtonClicked();

protected:
	AMSamplePlate *samplePlate_;
	AMSamplePlateItemModel *samplePlateItemModel_;
	AMSamplePlateItemModel *emptyModel_;
	QListView *sampleListView_;

	QVBoxLayout *vl_;
	QLabel *noSamplePlateLabel_;
	QHBoxLayout *samplePlateModifiedHL_;
	QLabel *samplePlateModifiedLabel_;
	QPushButton *saveSamplePlateButton_;

	QTimer *samplePlateModifiedTimer_;
};

#endif // AMSAMPLEPLATEVIEW_H
