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






/*!
  \todo
  - convert entire scroll area to QGraphicsView [in progress]
  - additional 3 views
  - buttons: expand all, collapse all, set default thumbnail, slider for size
  - progress bar or loading throbber (and process system events)
  - drag to experiment to add to exp.
  - double-click: open editor window (through AMAppController)
  - figure out item ownership when installed in layouts...
  */

namespace AMDataViews {
	enum ViewMode { ThumbnailView, ListView, FlowView, DetailView };
	enum OrganizeMode { OrganizeNone, OrganizeRuns, OrganizeExperiments, OrganizeScanTypes, OrganizeSamples, OrganizeElements };
}

/// All sub-sections in an AMDataView need to be QGraphicsLayoutItems. Additionally, they also need a setWidthConstraint function as defined by this interface.  This is a standard way of notifying them of the current width available inside the DataView. It will be called after creation, and also whenever the view resizes.
class AMAbstractDataViewSection {
public:
	virtual void setWidthConstraint(double widthConstraint) = 0;
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
	/// called when the widget is resized
	void onResize();

	/// called when the scene selection changes. We use this to determine the selected items that represent scan objects
	void onSceneSelectionChanged();
	/// Called when something in the scene is double-clicked. We use this to send the activated() signal
	void onSceneDoubleClicked();

	/// call this to adjust the accessible height of the scroll area to match the height required inside it
	void adjustViewScrollHeight();


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
	QGraphicsWidget* gwidget_;
	QGraphicsLinearLayout* sectionLayout_;


	/// This function runs everytime showRun() or showExperiment() is called, or a change is made to the OrganizeMode or ViewMode.  It re-populates the view from scratch.
	void refreshView();
	/// This helper function re-populates the comboBox of OrganizeMode choices, as appropriate for the current settings.
	void refreshOrganizeModeBox();
	/// This helper function retrieves the user's name from the database and forms the possesive form of it, storing in userName_;
	void retrieveUserName();

	/// The amount of room that views should assume to have available
	double effectiveWidth() const { return this->width() - 20; }


	/// Overidden so that we can notify the contents of the scroll area to change width with us.
	virtual void resizeEvent(QResizeEvent *event);



};



/// This class provides a view of one "section" of a multi-part database view.  What is shown inside the section depends on the filter specified using an SQL "WHERE" clause.  The section can be collapsed (hidden) or expanded (shown), to save screen real-estate and reduce the amount of data that must be loaded.
class AMDataViewSection : public QGraphicsWidget, public AMAbstractDataViewSection, private Ui::AMDataViewSection {
	Q_OBJECT

public:
	/// Create a new "section view" of the data set.  \c title and \c subtitle are used for the header. A chunk of an SQL search string, suitable for appending behind a WHERE keyword, should be contained in \c whereClause.  The section will create a view type based on the \c viewMode.  If you want this section initially collapsed (for performance reasons), specify \c expanded = false.
	explicit AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded = true, QGraphicsItem* parent = 0, double initialWidthConstraint = 400);
	virtual ~AMDataViewSection() {}

	void setWidthConstraint(double widthConstraint) {
		widthConstraint_ = widthConstraint;
		if(subview__)
			subview__->setWidthConstraint(widthConstraint_);
	}

public slots:
	/// Expand or collapse this section (ie: show or hide everything below the header)
	void expand(bool expanded = true);
	void collapse() { expand(false); }

protected:
	AMDatabase* db_;
	AMDataViews::ViewMode viewMode_;
	QString whereClause_, title_, subtitle_;
	bool expanded_;
	// for now, the section header we made in the UI editor is proxy-widgeted into the graphics view:
	QWidget* proxiedWidget_;
	QGraphicsLinearLayout* layout_;
	QGraphicsLayoutItem* subview_;
	AMAbstractDataViewSection* subview__;
	double widthConstraint_;

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

	void setWidthConstraint(double widthConstraint) {
		layout_->setWidthConstraint(widthConstraint);
	}

public slots:
	void setThumbnailWidth(int width) {
		setThumbnailWidth(double(width));
	}
	void setThumbnailWidth(double width);

protected slots:
	/// This slot is delay-called when the graphics view widget is resized... Inside, we update the internal widget in the scene to the same width, and just enough height as required.
	// void onResize();

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

	void setWidthConstraint(double widthConstraint) { Q_UNUSED(widthConstraint); }

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
