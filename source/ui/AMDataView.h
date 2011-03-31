/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "ui_AMDataViewSection.h"
#include "ui_AMDataViewEmptyHeader.h"
#include "dataman/AMDatabase.h"

#include <QGraphicsView>
#include "ui/AMSignallingGraphicsScene.h"
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#include <QUrl>

#include "ui/AMFlowGraphicsLayout.h"


#include <QDebug>



/*!
  \todo Fall 2010
  - convert entire scroll area to QGraphicsView [in progress]
  - additional 3 views
  - buttons: expand all, collapse all, set default thumbnail, slider for size
  - progress bar or loading throbber (and process system events)
  - drag to experiment to add to exp.
  - double-click: open editor window (through AMAppController)
  - figure out item ownership when installed in layouts...
  */


/// Support for "heightForWidth" mode is broken in QGraphicsLayout, at least up to Qt 4.7.2.  This is a workaround: a subclass of QGraphicsLinearLayout that lets you specify a width (and/or height) constraint. If either of these constraints are specified, then sizeHint() will always be called with these constraints.
/*! HeightForWidth mode means that the item's preferred height (returned via QGraphicsLayoutItem::sizeHint()) depends on the width available. (For example, a paragraph of wrapping text, or an image with a constant aspect ratio.)  It _should_ be activated by doing a setSizePolicy() on the item, where the size policy object has had setHeightForWidth(true) called on it.  When heightForWidth is set, layouts should always provide a width constraint when calling sizeHint(), but they don't.

(In Qt 4.7.1, they do, but it breaks spanning in row layouts. In Qt 4.7.2, it's "fixed", but layout performance is terrible, taking a minute or more with nested layouts.  See http://bugreports.qt.nokia.com/browse/QTBUG-12835 and http://bugreports.qt.nokia.com/browse/QTBUG-15333).

This is our workaround for the time-being.  Do not setHeightForWidth(true) on the QGraphicsLayoutItem's sizePolicy().  Instead, just call setSizeConstraint() on this top-level layout. (This will invalidate the layout if the new size constraint is different.) To remove a size constraint, specify an invalid width or height: for example, to un-set the width constraint, specify a setSizeConstraint(QSizeF(-1, whatever)).


Note: sizeHint() ignores the \constraint provided and replaces it with our internal constraint. Therefore, you should only use this layout as a top-level layout; child layouts will receive their size constraints through the sizeHint() function argument like normal.
*/
class AMConstrainedLinearLayout : public QGraphicsLinearLayout {
public:
	AMConstrainedLinearLayout( Qt::Orientation orientation, QSizeF initialConstraint = QSizeF(), QGraphicsLayoutItem * parent = 0 );

	/// Set the size constraint which will be inserted whenever sizeHint() is called.
	void setSizeConstraint(const QSizeF& newConstraint) {
		if(newConstraint == constraint_)
			return;

		constraint_ = newConstraint;
		updateGeometry();	// with a different sizeConstraint, our preferred geometry may have changed. This notifies any parent layouts, and clears the cached sizeHints.
	}

	/// Returns the user-enforced size constraint which is used for all calls to sizeHint().
	QSizeF sizeConstraint() const { return constraint_; }

protected:
	/// Re-implemented from QGraphicsLinearLayout to always enforce a size constraint.
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const {
		Q_UNUSED(constraint)

		qDebug() << "Calling top-level sizeHint with constraint_" << constraint_;

		return QGraphicsLinearLayout::sizeHint(which, constraint_);
	}

	QSizeF constraint_;
};


/// This subclass of QGraphicsWidget is designed to resize itself to the preferred sizeHint() of its layout.  (This is the opposite of what usually happens: layouts resize themselves to match the size of their widget.)
class AMLayoutControlledGraphicsWidget : public QGraphicsWidget {
	Q_OBJECT
public:
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

/// All sub-sections in an AMDataView need to be QGraphicsLayoutItems. Additionally, they also need a setWidthConstraint function as defined by this interface.  This is a standard way of notifying them of the current width available inside the DataView. It will be called after creation, and also whenever the view resizes.
class AMAbstractDataViewSection {
public:
	// virtual void setWidthConstraint(double widthConstraint) = 0;
};

/// This class implements a supremely awesome view of all the scan data found that can be found in a given database.  It can show scans from just one run, or just one experiment, or all runs or all experiments.  Beyond that, it can organize its data into sections based on the run, experiment, sample, element, or scan type.
/*! Implementation note: this class  acts as a QGraphicsView container and a controller for AMDataViewSections and the specialized views employed by them.

The container functionality consists of a QGraphicsView, holding a QGraphicsScene with one top-level QGraphicsWidget. That internal QGraphicsWidget is resized in width whenever the AMDataView widget is resized, so that contained views may expand or shrink in height but always fill the width exactly. Only a vertical scroll bar is needed to scroll through the data.

The controller functionality consists of responding to signals from the OrganizeMode combo box, and the showRun(), showExperiment(), setOrganizeMode(), and setViewMode() slots.  When this happens, the database is queried for the  information corresponding to the view requested, and AMDataViewSection views for each section are created to show the actual data representation.

Some careful programmers will suggest that the controller functionality and view functionality (especially the QGraphicsView/resizing widget system) should be split into separate classes. All the power to you ; )

Last of all, this class emits the selected(QList<QUrl>) and activated(QList<QUrl>) signals, to notify you when the user has selected or attempted to open a set of scans/objects. The format of the QUrls is the same as used for drag and drop events:
- amd://databaseConnectionName/tableName/objectId
*/
class AMDataView : public QWidget, private Ui::AMDataView
{
Q_OBJECT
public:
	/// Create an AMDataView widget, where \c database is the database you want to explore.
	explicit AMDataView(AMDatabase* database = AMDatabase::userdb(), QWidget *parent = 0);


signals:
	void selected(const QList<QUrl>&);
	void activated(const QList<QUrl>&);

public slots:
	/// setup this view to show a specific run (or use \c runId = -1 to see all runs)
	void showRun(int runId = -1);
	/// setup this view to show a specific experiment (or use \c experimentId = -1 to see all experiments)
	void showExperiment(int experimentId = -1);

	/// Set the headers used to separate and organize scans within a view. The \c mode is one of AMDataViews::OrganizeMode.
	void setOrganizeMode(int mode = AMDataViews::OrganizeNone);

	/// Set the view mode, where \c mode is one of AMDataViews::ViewMode (ThumbnailView, ListView, FlowView, or DetailView)
	void setViewMode(int mode);


protected slots:
	/// called when the combo box is used to change the organizeMode
	void onOrganizeModeBoxCurrentIndexChanged(int newIndex);
	/// called when the view is resized
	void onResize();

	/// called when the scene selection changes. We use this to determine the selected items that represent scan objects
	void onSceneSelectionChanged();
	/// Called when something in the scene is double-clicked. We use this to send the activated() signal
	void onSceneDoubleClicked();

	/// call this to adjust the accessible region of the graphicsview (gview_) to match the size of the graphicswidget gwidget_ inside it. (Called from a signal after the gwidget_ is resized.)
	void adjustViewScrollableArea();


protected:
	// Logic components:
	AMDataViews::ViewMode viewMode_;
	AMDataViews::OrganizeMode organizeMode_;
	int runId_, experimentId_;
	bool runOrExp_;

	/// all the items inside the different "sections" need to be QGraphicsLayoutItems, as they are part of a QGraphicsLinearLayout
	QList<QGraphicsLayoutItem*> sections_;
	/// They also need to be AMAbstractDataViewSections, to let us constrain their width when the size of the view changes. Since making AMAbstractDataViewSection inherit from QGraphicsLayoutItem would cause a host of virtual inheritance problems for real objects, instead we just store the pointers for each section twice, under both types.
	QList<AMAbstractDataViewSection*> sections__;

	/// The database explored with this view
	AMDatabase* db_;
	/// the user's name, with appropriate possesive ending to be tacked onto "Data".  (ie: Mark Boots's )
	QString userName_;

	/// The database id's of currently selected scans
	QList<int> selectedIds_;
	QStringList selectedTableNames_;
	QList<QUrl> selectedUrls_;
	/// This helper function packages the selectedIds_ and selectedTableNames_ into the selectedUrls_ format, and emits the selected() signal.
	void processNewSelectedIds();



	// UI components:
	QButtonGroup* viewModeButtonGroup_;

	// QGraphicsView UI components:

	QGraphicsView* gview_;
	AMSignallingGraphicsScene* gscene_;
	AMLayoutControlledGraphicsWidget* gwidget_;
	AMConstrainedLinearLayout* sectionLayout_;

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
class AMDataViewSection : public QGraphicsWidget, public AMAbstractDataViewSection, private Ui::AMDataViewSection {
	Q_OBJECT

public:
	/// Create a new "section view" of the data set.  \c title and \c subtitle are used for the header. A chunk of an SQL search string, suitable for appending behind a WHERE keyword, should be contained in \c whereClause.  The section will create a view type based on the \c viewMode.  If you want this section initially collapsed (for performance reasons), specify \c expanded = false.  \c parent is the QGraphicsItem parent for this section.
	explicit AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded = true, QGraphicsItem* parent = 0, double initialWidthConstraint = 400);
	virtual ~AMDataViewSection() {}

//	void setWidthConstraint(double widthConstraint) {
//		widthConstraint_ = widthConstraint;
//		if(subview__)
//			subview__->setWidthConstraint(widthConstraint_);
//		updateGeometry();
//	}


	/// Re-implemented function, to manage the size and position of our child items: the header, and the subview.
	void setGeometry(const QRectF &rect);

public slots:
	/// Expand or collapse this section (ie: show or hide everything below the header)
	void expand(bool expanded = true);
	void collapse() { expand(false); }

	/// Called from setGeometry(), this places the header at the correct location. It's a slot so that it can be connected to be called whenever the view scrolls.
	void layoutHeaderItem();

protected:
	AMDatabase* db_;
	AMDataViews::ViewMode viewMode_;
	QString whereClause_, title_, subtitle_;
	bool expanded_;
	// for now, the section header we made in the UI editor is proxy-widgeted into the graphics view:
	QWidget* proxiedWidget_;
	QGraphicsProxyWidget* proxyWidget_;

	QGraphicsLayoutItem* subview_;
	AMAbstractDataViewSection* subview__;
	// double widthConstraint_;

	/// Reimplemented from QGraphicsLayoutItem to combine the sizeHint() of our header and subview.
	QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
	/// Called from setGeometry(), this places the contents at the correct location
	void layoutContents();

	/// count the number of results that would appear for the given database and whereClause_.
	int countResults();
};

class AMFlowGraphicsLayout;
class AMThumbnailScrollGraphicsWidget;
#include <QGraphicsView>

/// This widget is used inside an AMDataViewSection to show the section's items using thumbnails.
class AMDataViewSectionThumbnailView : public QGraphicsWidget, public AMAbstractDataViewSection {
	Q_OBJECT

public:
	explicit AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QGraphicsItem* parent = 0, double initialWidthConstraint = 400);
	virtual ~AMDataViewSectionThumbnailView() {}

//	void setWidthConstraint(double widthConstraint) {
//		layout_->setWidthConstraint(widthConstraint);
//	}

public slots:
	void setThumbnailWidth(int width) {
		setThumbnailWidth(double(width));
	}
	void setThumbnailWidth(double width);

protected slots:

protected:
	AMDatabase* db_;
	QString dbTableName_;
	QString whereClause_;

	AMFlowGraphicsLayout* layout_;
	QList<AMThumbnailScrollGraphicsWidget*> thumbs_;
	double thumbnailWidth_;

	/// This helper function populates the view / layout with all of the thumbnails we need.
	void populate();

};


#include <QGraphicsProxyWidget>

class AMDataViewEmptyHeader : public QGraphicsWidget, public AMAbstractDataViewSection, private Ui::AMDataViewEmptyHeader {
	Q_OBJECT
public:
	explicit AMDataViewEmptyHeader(const QString& message, QGraphicsItem* parent = 0)
		: QGraphicsWidget(parent) {
		proxiedWidget_ = new QWidget();
		setupUi(proxiedWidget_);
		QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget(this);
		proxy->setWidget(proxiedWidget_);
		layout_ = new QGraphicsLinearLayout(Qt::Vertical);
		layout_->setContentsMargins(0,0,0,0);
		setLayout(layout_);
		layout_->addItem(proxy);

		messageText_->setText(message);
	}
	virtual ~AMDataViewEmptyHeader() {}

//	void setWidthConstraint(double widthConstraint) { Q_UNUSED(widthConstraint); }

	QString messageText() const { return messageText_->text(); }

public slots:
	void setMessageText(const QString& text) {
		messageText_->setText(text);
	}

protected:
	QWidget* proxiedWidget_;
	QGraphicsLinearLayout* layout_;

};

#endif // AMDATAVIEW_H
