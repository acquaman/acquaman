#ifndef ACQMAN_AMSIDEBAR_H
#define ACQMAN_AMSIDEBAR_H

#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVariant>

#include "acquaman.h"


#include "ui/AMDragDropItemModel.h"

/* Unused / removed...

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


#define AMSIDEBAR_SELECTOR_STYLESHEET " #AMSidebarSelectorText { font: 500 10pt \"Lucida Grande\"; color: rgb(0, 0, 0); } QFrame#AMSidebarSelector { padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border: 1px none transparent; }  	QFrame#AMSidebarSelector:hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); } #AMSidebarSelectorText[highlighted=\"true\"] { font: 500 10pt \"Lucida Grande\"; color: rgb(255, 255, 255); } QFrame#AMSidebarSelector[highlighted=\"true\"] { padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); color: rgb(255, 255, 255); } "

// removed from above: background-color: rgb(221, 227, 234);


#define AMSIDEBAR_HEADER_STYLESHEET "QLabel { padding-left: 10px; padding-right: 14px; padding-top: 8px; padding-bottom: 4px; font: bold 10pt \"Lucida Grande\"; color: rgb(100, 109, 125); }"

#define AMSIDEBAR_STYLESHEET " #AMSidebarFill { background-color: rgb(221, 227, 234); } AMSidebar { border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64); } "
  */


/// DEPRECATED CLASS -- An AMSidebarItem is a QStandardItem optimized for use as a "link" inside an AMSidebar. It has the usual name and icon, and it also has a QVariant \c link payload and a \c weight.  The weight is used in sorting (with lighter or more negative items on top), and the \c link is included as a parameter in the linkClicked() / linkDoubleClicked() signals emitted by AMSidebar.
/*! The weight is stored under a user-role AM::WeightRole, and the link stored under AM::LinkRole. */
class AMSidebarItem : public QStandardItem {
public:
	/// Default constructor
	explicit AMSidebarItem(const QString& name, const QVariant& link, const QIcon& icon = QIcon(), double weight = 0)
		: QStandardItem(icon, name) {
		setWeight(weight);
		setLink(link);
		setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	}

	void setWeight(double weight) {
		setData(weight, AM::WeightRole);
	}

	void setLink(const QVariant& link) {
		setData(link, AM::LinkRole);
	}


	/// Re-implemented from QStandardItem, this function sorts by weight in AM::WeightRole
	virtual bool operator< ( const QStandardItem & other ) const {
		return data(AM::WeightRole).toDouble() < other.data(AM::WeightRole).toDouble();
	}

	/// Re-implemented to designate as special type of QStandardItem
	virtual int type() const {
		return AM::SidebarItem;
	}


};

#include <QFont>
/// DEPRECATED CLASS -- AMSidebarHeadings are restricted version of AMSidebarItems, that are only used to act as headings (or category titles) in a sidebar.
class AMSidebarHeading : public AMSidebarItem {
public:
	explicit AMSidebarHeading(const QString& name, double weight = 0)
		: AMSidebarItem(name, QVariant(), QIcon(), weight) {
		setFlags(Qt::ItemIsEnabled);
		QFont font = QFont("Lucida Grande", 10, QFont::Bold);
		font.setCapitalization(QFont::AllUppercase);
		setFont(font);
		setData(QBrush(QColor::fromRgb(100, 109, 125)), Qt::ForegroundRole);
	}

	virtual int type() const {
		return AM::SidebarHeadingItem;
	}

};

/// DEPRECATED CLASS -- This class provides an iTunes/iPhoto style left sidebar, which contains a list of links.  It supports collapsable category headers, icons, and nested lists.
/*! Terminology:
	- Link: An entry in the list, representing some kind of action or result.  The link "payload" is a QVariant, and is up to the user.  When a link's selector is clicked/selected (or double-clicked), the linkSelected(QVariant) (or linkDoubleClicked(QVariant)) signal is emitted.
	- Heading: Links fall under headings (or categories), which are not (usually) active links, but can be collapsed to show or hide a group.
	- Weight: categories and selectors "settle" into their positions in the list by weight, with lightest (most negative) weights at the top, and heaviest (most positive) weights at the bottom. If specifying custom weights, we recommend using negative weights, since then the default weight (0) will always append ("sink" to the bottom).
*/
class AMSidebar : public QTreeView {

	Q_OBJECT

public:
	/// Default constructor
	explicit AMSidebar(QWidget* parent = 0);

	/// add a category header with a certain \c weight
	void addHeading(const QString& categoryName, double weight = 0);

	/// add a new link under category \c heading, with a payload \c linkContent. Returns a pointer to the item that was created.
	/*! If no category with this name exists, a new category is created.
	  */
	QStandardItem* addLink(const QString& heading, const QVariant& linkContent, const QString& text, const QIcon& icon = QIcon(), double weight = 0);

	/// add a new link (or group of links) under category \c heading, with a payload \c linkContent. The QStandardItem \c item is used to represent the top-level link.
	/*!  To create nested links, you can give \c item a set of sub-items with QStandardItem::addRow() before calling this function.  Be sure to manually set each item's AM::LinkRole to the desired destination first. */
	void addLink(const QString& heading, const QVariant& linkContent, QStandardItem* item, double weight = 0);

	/// Remove a link item
	void deleteLink(QStandardItem* item) {
		model_->removeRow(item->row(), item->parent()->index());
	}

	/// Access a QStandardItemModel containing all of the sidebar entries: call QTreeView::model().
	// QStandardItemModel* model() const;

	/// Access the heading item for a given category
	AMSidebarHeading* heading(const QString& headingTitle);

public slots:
	/// set the highlighted link.
	void setHighlightedLink(const QStandardItem* item) {
		setCurrentIndex(item->index());
	}

protected slots:
	void onItemDoubleClicked(const QModelIndex & index);
	virtual void currentChanged ( const QModelIndex & current, const QModelIndex & previous );

protected:

	// bool eventFilter(QObject* sourceObject, QEvent* event);

signals:
	void linkSelected(const QVariant& activatedLink);
	void linkDoubleClicked(const QVariant& activatedLink);


protected:

	AMDragDropItemModel* model_;
	QHash<QString,AMSidebarHeading*> headings_;


};

#endif
