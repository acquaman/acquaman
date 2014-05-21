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


#ifndef AMDATAVIEW_H
#define AMDATAVIEW_H

#include <QWidget>
#include "ui_AMDataView.h"
#include "ui_AMDataViewSectionHeader.h"
#include "ui_AMDataViewEmptyHeader.h"
#include "dataman/database/AMDatabase.h"
#include "util/AMDeferredFunctionCall.h"

#include <QGraphicsView>
#include "ui/AMSignallingGraphicsView.h"
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QUrl>
#include "ui/dataman/AMFlowGraphicsLayout.h"



/// This subclass of QGraphicsWidget is designed to resize itself to the preferred sizeHint() of its layout.  (This is the opposite of what usually happens: layouts resize themselves to match the size of their widget.)
class AMLayoutControlledGraphicsWidget : public QGraphicsWidget {
	Q_OBJECT
public:
 	virtual ~AMLayoutControlledGraphicsWidget();
	AMLayoutControlledGraphicsWidget(QGraphicsItem* parent = 0, Qt::WindowFlags wFlags = 0);

signals:
	/// Emitted after the widget is resized
	void resized();

protected:
	/// Re-implemented to catch the first LayoutEvent and resize ourself to match the layout's sizeHint (if necessary).
	virtual bool event(QEvent *event);

	/// Re-implemented to emit the resized() signal after handling
	virtual void resizeEvent(QGraphicsSceneResizeEvent *event);

};


namespace AMDataViews {
	enum ViewMode { ThumbnailView, ListView, FlowView, DetailView };
	enum OrganizeMode { OrganizeNone, OrganizeRuns, OrganizeExperiments, OrganizeScanTypes, OrganizeSamples, OrganizeElements };
}

/// All sub-sections in an AMDataView need to be an AMAbstractDataViewSection.
class AMAbstractDataViewSection : public QGraphicsWidget {
	Q_OBJECT
public:
 	virtual ~AMAbstractDataViewSection();
	AMAbstractDataViewSection(QGraphicsItem* parent = 0) : QGraphicsWidget(parent) {}

	/// Support for "heightForWidth" mode is broken in QGraphicsLayout, at least up to Qt 4.7.2.  This is a workaround: we create and call a setWidthConstraint() on all the items in the top-level layout; they use this width in their sizeHint() function if a constraint is not provided.
	/*! HeightForWidth mode means that the item's preferred height (returned via QGraphicsLayoutItem::sizeHint()) depends on the width available. (For example, a paragraph of wrapping text, or an image with a constant aspect ratio.)  It _should_ be activated by doing a setSizePolicy() on the item, where the size policy object has had setHeightForWidth(true) called on it.  When heightForWidth is set, layouts should always provide a width constraint when calling sizeHint(), but they don't.

	(In Qt 4.7.1, they do, but it breaks spanning in row layouts. In Qt 4.7.2, it's "fixed", but layout performance is terrible, taking a minute or more with nested layouts.  See http://bugreports.qt.nokia.com/browse/QTBUG-12835 and http://bugreports.qt.nokia.com/browse/QTBUG-15333).

	This is our workaround for the time-being.  Do not setHeightForWidth(true) on the QGraphicsLayoutItem's sizePolicy().  Instead, call setWidthConstraint() on all the items in the top-level layout.  This causes them to re-calculate their sizeHints(), after which they should call updateGeometry(), posting a bunch of LayoutRequest events that will be gathered into one overall re-layout of the master layout.
	*/
	virtual void setWidthConstraint(double widthConstraint) = 0;

	/// This is the interface used to tell the section views to set the size of their items.  It's up to them what the item size means... it's just  relative scale from 1 (smallest useful) to 100 (largest useful). The default implementation does nothing.
	virtual void setItemSize(int relativeItemSize) {
		Q_UNUSED(relativeItemSize)
	}

	/// Expand or collapse this section (ie: show or hide everything except for header). The default implementation does nothing and must be overridden if you have more than a header.
	virtual void expand(bool expanded = true) {
		Q_UNUSED(expanded)
	}
	/// Overloaded as an alternative for expand(false). You don't need to re-implement this.
	virtual void collapse() { expand(false); }

	/// Child sections that aren't part of a layout but directly managed by their parent... must call this on their parent whenever their sizeHint() has changed. The default version simply calls updateGeometry() at the parent level, which assumes that the parent's sizeHint() has automatically changed to reflect its child's new size.
	virtual void notifyThatChildSizeHintChanged() { updateGeometry(); }

	/// Access the currently-selected items (as a list of amd:// URLs).  Only meaningful for sections that know how to identify their selected items; the base implementation returns an empty list.
	virtual QList<QUrl> selectedItems() const { return QList<QUrl>(); }

	/// Retrieving selectedItems() could take time (especially if there are many many selected items), as it needs to build the URLs. This is a faster way to find out if any items are selected.  The base implementation returns 0.
	virtual int numberOfSelectedItems() const { return 0; }

signals:

	/// For sections that know how to identify their selected items, this is emitted when the selection changes.
	void selectionChanged();
};



/// This class implements a supremely awesome view of all the scan data found that can be found in a given database.  It can show scans from just one run, or just one experiment, or all runs or all experiments.  Beyond that, it can organize its data into sections based on the run, experiment, sample, element, or scan type.
/*! Implementation note: this class  acts as a QGraphicsView container, and a controller for AMDataViewSections and the specialized views employed by them.

The container functionality consists of a QGraphicsView, holding a QGraphicsScene with one top-level QGraphicsWidget. That internal QGraphicsWidget is resized in width whenever the AMDataView widget is resized, so that contained views may expand or shrink in height but always fill the width exactly. Only a vertical scroll bar is needed to scroll through the data.

The controller functionality consists of responding to signals from the OrganizeMode combo box, and the showRun(), showExperiment(), setOrganizeMode(), and setViewMode() slots.  When this happens, the database is queried for the  information corresponding to the view requested, and AMDataViewSection views for each section are created to show the actual data representation.

Some careful programmers will suggest that the controller functionality and view functionality (especially the QGraphicsView/resizing widget system) should be split into separate classes. All the power to you ; )

Last of all, this class emits the selectionChanged() and selectionActivated(QList<QUrl>) signals, to notify you when the user has selected or attempted to open a set of scans/objects. The format of the QUrls is the same as used for drag and drop events:
- amd://databaseConnectionName/tableName/objectId
*/
class AMDataView : public QWidget, protected Ui::AMDataView
{
Q_OBJECT
public:
	/// Create an AMDataView widget, where \c database is the database you want to explore.
	explicit AMDataView(AMDatabase* database = AMDatabase::database("user"), QWidget *parent = 0);


	/// Access a list of the selected items, in the standard URL format: amd://databaseConnectionName/tableName/objectId
	QList<QUrl> selectedItems() const { updateSelectedUrls(); return selectedUrls_; }

	/// Retrieving selectedItems() could take time (especially if there are many many selected items), as it needs to build the URLs. This is a faster way to find out if any items are selected.
	int numberOfSelectedItems() const;

signals:
	/// Emitted whenever the selected scans change.  You can then use selectedItems() to get a list of the scan URLs, or numberOfSelectedItems() to find out how many there are.
	void selectionChanged();
	/// Emitted when the user double-clicks anywhere in the view.
	void viewDoubleClicked();
	/// Emitted when the user chooses to open a scan configuration from the database.
	void launchScanConfigurationsFromDb();
	void launchScanConfigurationsFromDb(const QList<QUrl>&);
	/// Emitted when the user chooses to edit scans.
	void editScansFromDb();
	void editScansFromDb(const QList<QUrl>&);
	/// Emitted when the user chooses to compare scans.
	void compareScansFromDb();
	/// Emitted when the user wants to export scans.
	void exportScansFromDb();
	void exportScansFromDb(const QList<QUrl>&);
	/// Emitted when the user wants to fix a scan that uses CDF.
	void fixCDF();


public slots:
	/// setup this view to show a specific run (or use \c runId = -1 to see all runs)
	void showRun(int runId = -1);
	/// setup this view to show a specific experiment (or use \c experimentId = -1 to see all experiments)
	void showExperiment(int experimentId = -1);

	/// Set the headers used to separate and organize scans within a view. The \c mode is one of AMDataViews::OrganizeMode.
	void setOrganizeMode(int mode = AMDataViews::OrganizeNone);

	/// Set the view mode, where \c mode is one of AMDataViews::ViewMode (ThumbnailView, ListView, FlowView, or DetailView)
	void setViewMode(int mode);

	/// Called when the user wants to expand all the sections in the view
	void expandAll();
	/// Called when the user wants to collapse all the sections in the view
	void collapseAll();
	/// Called when the item size slider is moved. It's up to each data view section to decide what item sizes mean, but they should all adjust their item sizes based on the new user value (from 1 to 100).
	void setItemSize(int newItemSize);




protected slots:
	/// called when the combo box is used to change the organizeMode
	void onOrganizeModeBoxCurrentIndexChanged(int newIndex);
	/// called when the view is resized
	void onResize();

	/// called when the scene selection or item-views selection changes. Invalidates the selectedUrls_. When working with item-view based subviews, cancels the selection in other sections except the sender().
	void onScanItemsSelectionChanged();

	/// call this to adjust the accessible region of the graphicsview (gview_) to match the size of the graphicswidget gwidget_ inside it. (Called from a signal after the gwidget_ is resized.)
	void adjustViewScrollableArea();

	/// called when the scan search dialog emits editScanRequested()
	void onScanSearchEditScanRequested(const QList<QUrl>& scanUrls);
	void onScanSearchLaunchScanConfigurationRequested(const QList<QUrl>& scanUrls);
	void onScanSearchExportScanRequested(const QList<QUrl>& scanUrls);

	/// Drag handling
	void onDragStarted(const QPoint& startPos, const QPoint& currentPos);
	/// Drag handling
	void onDragMoved(const QPoint& startPos, const QPoint& currentPos);
	/// Drag handling
	void onDragEnded(const QPoint& startPos, const QPoint& endPos);
	/// Click handling: any click
	void onViewClicked(const QPoint& clickPos);
	void onViewDoubleClicked(const QPoint& clickPos);

	/// Builds a popup menu for switching view modes.
	void onCustomContextMenuRequested(QPoint pos);



	/// Connected to the database's created() and removed() signals, to catch when scans are added or removed.
	void onDatabaseItemCreatedOrRemoved(const QString& tableName, int id);
	/// Called when control returns to the event loop, after the scans table in the database has an item added or removed. Our response depends on whether the widget is visible or not.
	void afterDatabaseItemChanged();





protected:
	// Logic components:
	AMDataViews::ViewMode viewMode_;
	AMDataViews::OrganizeMode organizeMode_;
	int runId_, experimentId_;
	bool runOrExp_;


	/// All of the top-level sections in our view.
	QList<AMAbstractDataViewSection*> sections_;

	/// The database explored with this view
	AMDatabase* db_;
	/// the user's name, with appropriate possesive ending to be tacked onto "Data".  (ie: John Doe's )
	QString userName_;
	/// Caches the name of the scans table in the database
	QString scansTableName_;


	// Selection Management
	///////////////////////////////
	/// A prepared list of the selected items, as amd://databaseConnectionName/tableName/objectId URLs.
	mutable QList<QUrl> selectedUrls_;
	/// True if the selection has changed and selectedUrls_ is dirty.
	mutable bool selectedUrlsUpdateRequired_;
	/// This helper function analyzes the current selection and updates selectedUrls_;
	void updateSelectedUrls() const;


	/// Used for drawing a rubber-band area when handling drags.
	QGraphicsRectItem* rubberBand_;
	QPainterPath selectionArea_;
	/// Helper function to start a QDrag with all the selected items. (ie: for dragging scans to the runs, experiments, or trash). \c optionalPixmap is a thumbnail to represent the drag.
	void startDragWithSelectedItems(const QPixmap& optionalPixmap = QPixmap());

	/// In thumbnail mode, we want to manage selection ourselves... and most of all, we want to stop the scene from selecting objects when the mouse is first pressed. We do this with an event filter installed on the scene.
	bool eventFilter(QObject *, QEvent *);



	// UI components:
	///////////////////
	QButtonGroup* viewModeButtonGroup_;

	// QGraphicsView UI components:
	///////////////////
	AMSignallingGraphicsView* gview_;
	QGraphicsScene* gscene_;
	AMLayoutControlledGraphicsWidget* gwidget_;
	QGraphicsLinearLayout* sectionLayout_;

	int itemSize_;	///< Default item size for new sections


	// Catching and responding to database updates (scans added or removed)
	/////////////////////////
	AMDeferredFunctionCall afterDbChangedFnCall_;
	bool viewRequiresRefresh_;


	// Helper Functions
	/////////////////////////

	/// This function runs everytime showRun() or showExperiment() is called, or a change is made to the OrganizeMode or ViewMode.  It re-populates the view from scratch.
	void refreshView();
	/// This helper function re-populates the comboBox of OrganizeMode choices, as appropriate for the current settings.
	void refreshOrganizeModeBox();
	/// This helper function retrieves the user's name from the database and forms the possesive form of it, storing in userName_;
	void retrieveUserName();

	/// The amount of room that views should assume to have available
	double effectiveWidth() const { return this->width() - 20; }


	/// Overidden so that we can notify the contents _inside_ the scroll area to change width with us.
	virtual void resizeEvent(QResizeEvent *event);






};



/// This class provides a view of one "section" of a multi-part database view.  What is shown inside the section depends on the filter specified using an SQL "WHERE" clause.  The section can be collapsed (hidden) or expanded (shown), to save screen real-estate and reduce the amount of data that must be loaded.
class AMDataViewSection : public AMAbstractDataViewSection, private Ui::AMDataViewSectionHeader {
	Q_OBJECT

public:
	/// Create a new "section view" of the data set.  \c title and \c subtitle are used for the header. A chunk of an SQL search string, suitable for appending behind a WHERE keyword, should be contained in \c whereClause.  The section will create a view type based on the \c viewMode.  If you want this section initially collapsed (for performance reasons), specify \c expanded = false.  \c parent is the QGraphicsItem parent for this section.
	explicit AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded = true, QGraphicsItem* parent = 0, double initialWidthConstraint = 400, int initialItemSize = 50);
	virtual ~AMDataViewSection() {}

	void setWidthConstraint(double widthConstraint) {
		widthConstraint_ = widthConstraint;
		if(subview_)
			subview_->setWidthConstraint(widthConstraint_);
		updateGeometry();
	}


	void setItemSize(int relativeItemSize) {
		itemSize_ = relativeItemSize;
		if(subview_) {
			subview_->setItemSize(itemSize_);
		}
	}

	virtual QList<QUrl> selectedItems() const {
		if(subview_)
			return subview_->selectedItems();
		return QList<QUrl>();
	}

	virtual int numberOfSelectedItems() const {
		if(subview_)
			return subview_->numberOfSelectedItems();
		else
			return 0;
	}

	/// Re-implemented function, to manage the size and position of our child items: the header, and the subview.
	void setGeometry(const QRectF &rect);

public slots:
	/// Expand or collapse this section (ie: show or hide everything below the header).
	virtual void expand(bool expanded = true);

	/// Called from setGeometry(), this places the header at the correct location. It's a slot so that it can be connected to be called whenever the view scrolls.
	void layoutHeaderItem();

protected:
	AMDatabase* db_;
	AMDataViews::ViewMode viewMode_;
	QString whereClause_, title_, subtitle_;
	bool expanded_;
	// for now, the section header we made in the UI editor is proxy-widgeted into the graphics view:
	QFrame* proxiedWidget_;
	QGraphicsProxyWidget* proxyWidget_;

	AMAbstractDataViewSection* subview_;
	 double widthConstraint_;
	 int itemSize_;

	/// Reimplemented from QGraphicsLayoutItem to combine the sizeHint() of our header and subview.
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
	/// Called from setGeometry(), this places the contents at the correct location
	void layoutContents();

	/// count the number of results that would appear for the given database and whereClause_.
	int countResults();
};

class AMFlowGraphicsLayout;
class AMThumbnailScrollGraphicsWidget;

/// This widget is used inside an AMDataViewSection to show the section's items using thumbnails.
class AMDataViewSectionThumbnailView : public AMAbstractDataViewSection {
	Q_OBJECT

public:
	explicit AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent = 0, double initialWidthConstraint = 400, int initialItemSize = 50);
	virtual ~AMDataViewSectionThumbnailView() {}

	void setWidthConstraint(double widthConstraint) {
		layout_->setDefaultWidth(widthConstraint);
	}

	/// We use relativeItemSize to scale the thumbnails from 80px wide to ___ wide.  The \c relativeItemSize is from 0 to 100, where 50 is normal, 0 is "smallest useful", and 100 is "largest useful".
	void setItemSize(int relativeItemSize) {
		int width = int((double)relativeItemSize/50.0*160.0 + 80);
		layout_->setItemSizeConstraint(QSizeF(width, -1));	// this will change our size hint

		AMAbstractDataViewSection* parent = qobject_cast<AMAbstractDataViewSection*>(parentWidget());
		if(parent)
			parent->notifyThatChildSizeHintChanged();
	}

protected slots:

protected:
	AMDatabase* db_;
	QString dbTableName_;
	QString whereClause_;

	AMFlowGraphicsLayout* layout_;
	QList<AMThumbnailScrollGraphicsWidget*> thumbs_;

	/// This helper function populates the view / layout with all of the thumbnails we need.
	void populate();

};

#include <QTableView>
#include <QKeyEvent>

/// This QTableView is reimplemented to ignore scroll events
class AMIgnoreScrollTableView : public QTableView {
	Q_OBJECT

public:
 	virtual ~AMIgnoreScrollTableView();
	AMIgnoreScrollTableView(QWidget* parent = 0) : QTableView(parent) {}

	virtual bool event(QEvent *event);
};

#include "dataman/database/AMQueryTableModel.h"

/// This widget is used inside an AMDataViewSection to show the section's items in a table (list, or detail) view.
class AMDataViewSectionListView : public AMAbstractDataViewSection {
	Q_OBJECT

public:
	explicit AMDataViewSectionListView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent = 0, double initialWidthConstraint = 400, int initialItemSize = 50);

	virtual ~AMDataViewSectionListView() {
		delete tableView_;
		delete tableModel_;
	}

	virtual void setWidthConstraint(double widthConstraint);

	/// Returns the currently selected items, in the amd://databaseConnectionName/tableName/objectId URL format.
	virtual QList<QUrl> selectedItems() const;

	/// Retrieving selectedItems() could take time (especially if there are many many selected items), as it needs to build the URLs. This is a faster way to find out if any items are selected.
	virtual int numberOfSelectedItems() const;

protected slots:
	/// The AMScanQueryModel loads a maximum of 256 rows at once... and this is what will be loaded when we call refreshQuery().  We need to catch the signal when rows are inserted in the model, and use this to signal that our desired sizeHint has changed.
	void onRowsAddedOrRemoved();

protected:

protected:
	QGraphicsProxyWidget* proxyWidget_;
	AMIgnoreScrollTableView* tableView_;
	AMQueryTableModel* tableModel_;

	QGraphicsLinearLayout* layout_;

	double widthConstraint_;

	AMDatabase* db_;
	QString tableName_, whereClause_;

	/// Re-implemented to return a height based on the model's rowCount() and the height of each row (according to the QHeaderView). This ensures that we ask to be large enough to fit the whole view, without scroll bars.
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;


};

#include <QGraphicsProxyWidget>

class AMDataViewEmptyHeader : public AMAbstractDataViewSection, private Ui::AMDataViewEmptyHeader {
	Q_OBJECT
public:
	explicit AMDataViewEmptyHeader(const QString& message, double initialWidthConstraint = 400, QGraphicsItem* parent = 0);
	virtual ~AMDataViewEmptyHeader() {}

	void setWidthConstraint(double widthConstraint) { widthConstraint_ = widthConstraint; updateGeometry(); }

	QString messageText() const { return messageText_->text(); }

public slots:
	void setMessageText(const QString& text) {
		messageText_->setText(text);
	}

protected:
	QFrame* proxiedWidget_;
	QGraphicsLinearLayout* layout_;

	double widthConstraint_;

	/// Re-implemented to return a sizeHint that is exactly what the top widget wants us to be.
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
		Q_UNUSED(which)
		Q_UNUSED(constraint)

		return QSizeF(widthConstraint_, -1);
	}

};

#endif // AMDATAVIEW_H
