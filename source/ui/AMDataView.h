#ifndef AMDATAVIEW_H
#define AMDATAVIEW_H

#include <QWidget>
#include "ui_AMDataView.h"
#include "ui_AMDataViewSection.h"
#include "ui_AMDataViewEmptyHeader.h"
#include "dataman/AMDatabase.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

/*!
  \todo
  - convert entire scroll area to QGraphicsView
  - additional 3 views
  - buttons: expand all, collapse all, set default thumbnail, slider for size
  - progress bar or loading throbber (and process system events)
  - drag to experiment to add to exp.
  - double-click: open editor window (through AMAppController)
  */

namespace AMDataViews {
	enum ViewMode { ThumbnailView, ListView, FlowView, DetailView };
	enum OrganizeMode { OrganizeNone, OrganizeRuns, OrganizeExperiments, OrganizeScanTypes, OrganizeSamples, OrganizeElements };
}

class AMDataViewSection;

class AMDataView : public QWidget, private Ui::AMDataView
{
Q_OBJECT
public:
	/// Create an AMDataView widget, where \c database is the database you want to explore.
	explicit AMDataView(AMDatabase* database = AMDatabase::userdb(), QWidget *parent = 0);

signals:

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

protected:
	// Logic components:
	AMDataViews::ViewMode viewMode_;
	AMDataViews::OrganizeMode organizeMode_;
	int runId_, experimentId_;
	bool runOrExp_;

	QList<QWidget*> sections_;

	/// The database explored with this view
	AMDatabase* db_;
	/// the user's name, with appropriate possesive ending to be tacked onto "Data".  (ie: Mark Boots's )
	QString userName_;



	// UI components:
	QButtonGroup* viewModeButtonGroup_;
	QGraphicsView* gview_;
	QGraphicsScene* gscene_;
	QGraphicsWidget* gwidget_;
	QGraphicsLinearLayout* sectionLayout_;


	/// This function runs everytime showRun() or showExperiment() is called, or a change is made to the OrganizeMode or ViewMode.  It re-populates the view from scratch.
	void refreshView();
	/// This helper function re-populates the comboBox of OrganizeMode choices, as appropriate for the current settings.
	void refreshOrganizeModeBox();
	/// This helper function retrieves the user's name from the database and forms the possesive form of it, storing in userName_;
	void retrieveUserName();

	/// Overidden so that we can notify the contents of the scroll area to change width with us.
	virtual void resizeEvent(QResizeEvent *event);



};



/// This class provides a view of one "section" of a multi-part database view.  What is shown inside the section depends on the filter specified using an SQL "WHERE" clause.  The section can be collapsed (hidden) or expanded (shown), to save screen real-estate and reduce the amount of data that must be loaded.
class AMDataViewSection : public QWidget, private Ui::AMDataViewSection {
	Q_OBJECT

public:
	/// Create a new "section view" of the data set.  \c title and \c subtitle are used for the header. A section of an SQL search string, suitable for appending behind a WHERE keyword, should be contained in \c whereClause.  The section will create a view type based on the \c viewMode.  If you want this section initially collapsed (for performance reasons), specify \c expanded = false.
	explicit AMDataViewSection(const QString& title, const QString& subtitle, const QString& whereClause, AMDataViews::ViewMode viewMode, AMDatabase* db, bool expanded = true, QWidget* parent = 0);

public slots:
	/// Expand or collapse this section (ie: show or hide everything below the header)
	void expand(bool expanded = true);
	void collapse() { expand(false); }

protected:
	AMDatabase* db_;
	AMDataViews::ViewMode viewMode_;
	QString whereClause_;
	bool expanded_;
	QWidget* subview_;
};

class AMFlowGraphicsLayout;
class AMThumbnailScrollGraphicsWidget;
#include <QGraphicsView>

/// This widget is used inside an AMDataViewSection to show the section's items using thumbnails.
class AMDataViewSectionThumbnailView : public QGraphicsView {
	Q_OBJECT

public:
	explicit AMDataViewSectionThumbnailView(AMDatabase* db, const QString& dbTableName, const QString& whereClause, QWidget* parent = 0);

public slots:
	void setThumbnailWidth(int width) {
		setThumbnailWidth(double(width));
	}
	void setThumbnailWidth(double width);

protected slots:
	/// This slot is delay-called when the graphics view widget is resized... Inside, we update the internal widget in the scene to the same width, and just enough height as required.
	void onResize();

protected:
	AMDatabase* db_;
	QString dbTableName_;
	QString whereClause_;
	QGraphicsScene* scene_;
	QGraphicsWidget* gWidget_;
	AMFlowGraphicsLayout* layout_;
	QList<AMThumbnailScrollGraphicsWidget*> thumbs_;
	double thumbnailWidth_;

	/// This helper function populates the view / layout with all of the thumbnails we need.
	void populate();

	/// This event handler updates the size of the main graphics widget inside our view/scene, when the size of the view changes.
	virtual void resizeEvent(QResizeEvent *event);
};



class AMDataViewEmptyHeader : public QWidget, private Ui::AMDataViewEmptyHeader {
	Q_OBJECT
public:
	explicit AMDataViewEmptyHeader(const QString& message, QWidget* parent = 0)
		: QWidget(parent) {
		setupUi(this);
		messageText_->setText(message);
	}

	QString messageText() const { return messageText_->text(); }

public slots:
	void setMessageText(const QString& text) {
		messageText_->setText(text);
	}

};

#endif // AMDATAVIEW_H
