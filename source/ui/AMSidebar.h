#ifndef ACQMAN_AMSIDEBAR_H
#define ACQMAN_AMSIDEBAR_H

#include <QScrollArea>
#include <QFrame>
#include <QLabel>
#include <QHash>
#include <QVBoxLayout>
#include <QHash>
#include <QList>
#include <QEvent>
#include <QVariant>

#include <QDebug>


#define AMSIDEBAR_BG_COLOR 221, 227, 234
#define AMSIDEBAR_BORDER_COLOR 64, 64, 64

#define AMSIDEBAR_SELECTED_COLOR1 131, 137, 167
#define AMSIDEBAR_SELECTED_COLOR2 170, 176, 197
#define AMSIDEBAR_SELECTED_COLOR3 115, 122, 153

#define AMSIDEBAR_HIGHLIGHTED_COLOR1 69, 128, 200
#define AMSIDEBAR_HIGHLIGHTED_COLOR2 91, 146, 213
#define AMSIDEBAR_HIGHLIGHTED_COLOR3 22, 84, 170

#define AMSIDEBAR_MIN_HEIGHT 30
#define AMSIDEBAR_ICON_SIZE 22

#define AMSIDEBAR_SELECTOR_STYLESHEET " * { font: 500 10pt \"Lucida Grande\"; color: rgb(0, 0, 0); } QFrame#AMSidebarSelector { padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border: 0px solid black; }  	QFrame#AMSidebarSelector:hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); } "

// removed from above: background-color: rgb(221, 227, 234);

#define AMSIDEBAR_HIGHLIGHTED_STYLESHEET " * { font: 500 10pt \"Lucida Grande\"; color: rgb(255, 255, 255); } QFrame#AMSidebarSelector { padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border: 0px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); color: rgb(255, 255, 255); } "

#define AMSIDEBAR_HEADER_STYLESHEET "QLabel { padding-left: 10px; padding-right: 14px; padding-top: 8px; padding-bottom: 4px; font: bold 10pt \"Lucida Grande\"; color: rgb(100, 109, 125); }"

#define AMSIDEBAR_STYLESHEET " #AMSidebarFill { background-color: rgb(221, 227, 234); } AMSidebar { border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64); } "


class AMSidebarDefaultSelector : public QFrame {

public:
	explicit AMSidebarDefaultSelector(const QString& text = QString(), const QString& iconFileName = QString(), QWidget* parent = 0);

	QString text() const { return text_->text(); }
	void setText(const QString& text) { text_->setText(text); }


	/// Sets the icon displayed next to the label. We take ownership of the new icon, and delete our old one.
	void setIcon(const QString& fileName) {
		icon_->setPixmap(QPixmap(fileName));
	}

protected:

	QLabel* text_;
	QLabel* icon_;

	QHBoxLayout* hl_;
};


/// AMSidebar maintains a set of AMSidebarLink's, indexed by selector widget
class AMSidebarLink {
public:
	AMSidebarLink(QWidget* selector = 0, const QVariant& payload = QVariant(), double weight = 0, const QString& category = QString()) {
		selector_ = selector;
		payload_ = payload;
		weight_ = weight;
		category_ = category;
	}

	QWidget* selector_;
	QVariant payload_;
	double weight_;
	QString category_;
};

/// AMSidebar maintains a set of AMSidebarCategory's (indexed by category name)
class AMSidebarCategory : public QWidget {
public:
	AMSidebarCategory(const QString& categoryName, double weight, QWidget* parent);

	void addLink(const AMSidebarLink& addMe);
	void deleteLink(const AMSidebarLink& deleteMe);

protected:

	QString categoryName_;
	double weight_;
	QVBoxLayout* layout_;

	QList<double> linkWeights_;
};

/// This class provides an iTunes/iPhoto style left sidebar, which contains a list of links.  It supports category headers, icons, and (todo) nested lists.
/*! Terminology:
	- Selector: a widget used to select (ie: click), a link from the list
	- Link: An entry in the list, representing some kind of action or result.  The link "payload" is a QVariant, and is up to the user.  When a link's selector is clicked (or double-clicked), the linkClicked(QVariant) (or linkDoubleClicked(QVariant)) signal is emitted.
	- Category: Links fall under headings, or categories.
	- Weight: categories and selectors "settle" into their positions in the list by weight, with lightest (most negative) weights at the top, and heaviest (most positive) weights at the bottom. Recommend specifying negative weights, since then the default weight (0) will always append ("sink") to the bottom.
*/
class AMSidebar : public QScrollArea {
	
	Q_OBJECT
	
public:
	/// Default constructor
	explicit AMSidebar(QWidget* parent = 0);
	
	/// add a category header with a certain \c weight
	void addCategory(const QString& categoryName, double weight = 0);
	
	/// add a new link under category \c categoryName, with a payload \c linkContent.  The widget \c selector is used to represent the link in the list.
	/*! This widget takes ownership of \c selector.  If no category with this name exists, a new category is created.
	  */
	void addLink(const QString& categoryName, QVariant linkContent, QWidget* selector, double weight = 0);

	/// add a new link under category \c categoryName, with a payload \c linkContent.  A default widget with \c text and \c iconFileName is used to represent the link in the list.
	/*! If no category with this name exists, a new category is created.
	  */
	void addLink(const QString& categoryName, QVariant linkContent, const QString& text, const QString& iconFileName = "", double weight = 0);
	
	/// deletes the first link with a payload matching \c linkTarget
	void deleteLink(const QVariant& linkTarget);

public slots:
	/// set the highlighted link (specified by the selector widget).
	void setHighlightedLink(QWidget* selector);
	/// set the highlighted link (specified by the link content).  This version is slower than setHighlightedLink(QWidget* selector).
	void setHighlightedLink(QVariant linkContent);

protected:

	bool eventFilter(QObject* sourceObject, QEvent* event);
		
signals:
	void linkClicked(const QVariant& activatedLink);
	void linkDoubleClicked(const QVariant& activatedLink);
	
	
protected:
	
	QFrame* mainWidget_;
	QVBoxLayout* topLayout_;
	
	QHash<QString, AMSidebarCategory*> name2category_;
	QList<double> categoryWeights_;

	QHash<QWidget*, AMSidebarLink> selector2link_;
	

	
	QWidget* highlightedSelector_;

	
	bool doubleClickInProgress_;
	
	
};

#endif
