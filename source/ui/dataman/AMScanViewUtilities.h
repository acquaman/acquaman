/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSCANVIEWUTILITIES_H
#define AMSCANVIEWUTILITIES_H

#include <QMenu>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QGraphicsGridLayout>
#include <QButtonGroup>

#include "dataman/AMScanSetModel.h"
#include "ui/dataman/AMCramBarHorizontal.h"

/// This file contains some common classes that are used with AMScanView classes.

/// This menu subclass provides a context menu for AMScanViewScanBar
class AMScanViewScanBarContextMenu : public QMenu {
	Q_OBJECT
public:
	AMScanViewScanBarContextMenu(AMScanSetModel* model, int scanIndex, int dataSourceIndex, QWidget* parent = 0);
	~AMScanViewScanBarContextMenu();

protected:
	/// The model this context menu was created within
	AMScanSetModel* model_;
	/// index of the data source this context menu was created for. We need to use persistent indexes because the data sources might (very unlikely, but possibly) have been deleted while the right-click menu was open.
	QPersistentModelIndex pi_;


protected slots:
	/// Called when the "hide all data sources except this one" action is triggered.
	void hideAllExceptDataSource();
	/// Called when the "show all data sources like this one" action is triggered.
	void showAllDataSource();
	/// Called when the "show all data sources" action is triggered.  This only shows all the data sources for the scan that was selected.  If you want to show all data sources on all open scans then you need to do this for each scan.
	void showAll();
	/// Called when the "edit Color And Style" action is triggered.
	void editColorAndStyle();

};

/// This GUI class is a helper for AMScanViewSourceSelector.  It diplays the available data sources for a single Scan.
class AMScanViewScanBar : public QFrame {
	Q_OBJECT
public:
	explicit AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent = 0);

public slots:
	/// The ScanBar has two behaviours.  When exclusiveMode is on, it only allows one data source to be "checked" or selected at a time, and tells the model to make this the exclusive data source.  Otherwise, it allows multiple data sources to be checked, and toggles their visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected:
	/// ui components:
	QLabel* nameLabel_;
	QButtonGroup sourceButtons_;
	// REMOVED: QToolButton* closeButton_;
	AMCramBarHorizontal* cramBar_;

	/// Index of "our" scan in the model:
	int scanIndex_;
	/// Connected model:
	AMScanSetModel* model_;
	/// whether in exclusiveMode (ie: only one data source allowed) or not:
	bool exclusiveModeOn_;

protected slots:
	/// after a scan or data source is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);
	/// before a scan or data source is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);
	/// after a scan or data source is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end);
	/// when data changes:
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
	/// when the model's exclusivedata source is changed
	void onExclusiveDataSourceChanged(const QString& exclusiveDataSourceName);
	/// when one of the data source toggles is clicked:
	void onSourceButtonClicked(int id);

	/// called when a right-click menu is requested on any of the buttons. Produces a context menu. (\c location is in source button coordinates)
	void onDataSourceButtonRightClicked(const QPoint& location);

	friend class AMScanViewSourceSelector;
};

/// This GUI class is a view on an AMScanSetModel.  It shows each scan in a horizontal bar, with checkable buttons for each data source.
class AMScanViewSourceSelector : public QWidget {
	Q_OBJECT

public:
	explicit AMScanViewSourceSelector(AMScanSetModel* model = 0, QWidget* parent = 0);
	void setModel(AMScanSetModel* model);

public slots:
	/// ScanBars have two behaviours.  When exclusiveMode is on, they only allow one data source to be "checked" or selected at a time, and tell the model to make this the exclusive data source.  Otherwise, they allows multiple data sources within each Scan to be checked, and toggle the data sources' visibility in the model.
	void setExclusiveModeOn(bool exclusiveModeOn = true);

protected slots:
	/// after a scan or data source is added in the model
	void onRowInserted(const QModelIndex& parent, int start, int end);

	/// before a scan or data source is deleted in the model:
	void onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end);

	/// after a scan or data source is deleted in the model:
	void onRowRemoved(const QModelIndex& parent, int start, int end) {
		Q_UNUSED(parent)
		Q_UNUSED(start)
		Q_UNUSED(end)
		/// \todo Anything needed here?
		}

	/// when data within the model changes. Possibilities we care about: nothing. (All handled within AMScanViewScanBars.)
	void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {
		Q_UNUSED(topLeft)
		Q_UNUSED(bottomRight)
		}

protected:
	QList<AMScanViewScanBar*> scanBars_;
	QVBoxLayout* barLayout_;
	AMScanSetModel* model_;
	bool exclusiveModeOn_;
};

/// A GUI class that is a QGraphicsView, and provides a top-level QGraphicsWidget inside a scene.  It emits resized(const QSizeF& newSize) when the QGraphicsView widget is resized.
class AMGraphicsViewAndWidget : public QGraphicsView {
	Q_OBJECT
public:

	AMGraphicsViewAndWidget(QWidget* parent = 0) : QGraphicsView(parent) {

		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing /*| QPainter::HighQualityAntialiasing*/);

		scene_ = new QGraphicsScene();
		setScene(scene_);
		/// \bug testing if disabling item indexing solves anything...
		scene_->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsWidget_ = new QGraphicsWidget();
		graphicsWidget_->setGeometry(0,0,640,480);
		scene_->addItem(graphicsWidget_);

	}


	virtual ~AMGraphicsViewAndWidget() {
		delete graphicsWidget_;
		delete scene_;
	}

	QGraphicsWidget* graphicsWidget() const { return graphicsWidget_;}

signals:
	void resized(const QSizeF& newSize);

protected:
	QGraphicsScene* scene_;
	QGraphicsWidget* graphicsWidget_;

	// On resize events: notify the graphics widget to resize it.
	virtual void resizeEvent ( QResizeEvent * event ) {

		QGraphicsView::resizeEvent(event);
		emit resized(QSizeF(event->size()));
	}
};

#endif // AMSCANVIEWUTILITIES_H
