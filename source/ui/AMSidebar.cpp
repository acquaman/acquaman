#include "AMSidebar.h"

AMSidebarDefaultSelector::AMSidebarDefaultSelector(const QString& text, const QString& iconFileName, QWidget* parent)
	: QFrame(parent) {

	hl_ = new QHBoxLayout();
	hl_->setMargin(4);
	setLayout(hl_);

	icon_ = new QLabel();
	icon_->setFixedSize(16,16);
	icon_->setScaledContents(true);
	hl_->addWidget(icon_);

	if(!iconFileName.isEmpty())
		setIcon(iconFileName);

	text_ = new QLabel(text);
	text_->setObjectName("AMSidebarSelectorText");
	hl_->addWidget(text_);

	/// Set your object name to "AMSidebarSelector" to have your colors and background set automatically when mouse-overed/selected. If you don't, you're in charge of managing your colors yourself.
	setObjectName("AMSidebarSelector");

}

AMSidebarCategory::AMSidebarCategory(const QString& categoryName, double weight = 0, QWidget* parent = 0)
	: QWidget(parent)
{
	categoryName_ = categoryName;
	weight_ = weight;
	layout_ = new QVBoxLayout();
	layout_->setMargin(0);
	layout_->setSpacing(0);
	setLayout(layout_);

	QLabel* label = new QLabel(categoryName.toUpper());
	label->setStyleSheet(AMSIDEBAR_HEADER_STYLESHEET);
	layout_->addWidget(label);
}

void AMSidebarCategory::addLink(const AMSidebarLink& addMe) {
	int li;
	for(li=0; li<linkWeights_.count(); li++) {
		if(addMe.weight_ < linkWeights_.at(li)) {
			break;
		}
	}
	layout_->insertWidget(li+1, addMe.selector_);
	linkWeights_.insert(li, addMe.weight_);
}
void AMSidebarCategory::deleteLink(const AMSidebarLink& deleteMe) {

	delete deleteMe.selector_;
	int li;
	for(li=0; li<linkWeights_.count(); li++) {
		if(deleteMe.weight_ < linkWeights_.at(li)) {
			break;
		}
	}
	if(--li >= 0)
		linkWeights_.removeAt(li);
}



AMSidebar::AMSidebar(QWidget* parent) : QScrollArea(parent) {

	mainWidget_ = new QFrame(this);
	//mainWidget_->setMinimumWidth(200)

	topLayout_ = new QVBoxLayout();
	mainWidget_->setLayout(topLayout_);
	topLayout_->setSpacing(12);
	topLayout_->setContentsMargins(0, 0, 0, 0);
	/// \todo needed for sizing inside scroll area? topLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);


	// Add an expanding space at the bottom...
	topLayout_->addStretch();

	mainWidget_->setObjectName("AMSidebarFill");
	setObjectName("AMSidebarFill");
	setStyleSheet(AMSIDEBAR_STYLESHEET);

	highlightedSelector_ = 0;

	// scroll area:
	setWidgetResizable(true);
	setWidget(mainWidget_);
	mainWidget_->show();

	setMinimumWidth(200);


}

/// add a category header with a certain \c weight
void AMSidebar::addCategory(const QString& categoryName, double weight) {

	// If this category already exists...
	if (name2category_.contains(categoryName))
		return;

	AMSidebarCategory* c = new AMSidebarCategory(categoryName, weight);
	name2category_.insert(categoryName, c);

	int li;
	for(li=0; li<categoryWeights_.count(); li++) {
		if(weight < categoryWeights_.at(li)) {
			break;
		}
	}
	topLayout_->insertWidget(li, c);
	categoryWeights_.insert(li, weight);
}

/// add a new link under category \c categoryName, with a payload \c linkContent.  The widget \c selector is used to represent the link in the list.
/*! This widget takes ownership of \c selector.  If no category with this name exists, a new category is created.
  */
void AMSidebar::addLink(const QString& categoryName, QVariant linkContent, QWidget* selector, double weight) {

	if( !name2category_.contains(categoryName) )
		addCategory(categoryName);


	// selector->setObjectName("AMSidebarSelector");
	selector->setStyleSheet(selector->styleSheet() + AMSIDEBAR_SELECTOR_STYLESHEET);
	AMSidebarLink newLink(selector, linkContent, weight, categoryName);

	name2category_[categoryName]->addLink(newLink);
	selector2link_.insert(selector, newLink);
	// catch click and double-click events for the selector widget:
	selector->installEventFilter(this);

}

/// add a new link under category \c categoryName, with a payload \c linkContent.  A default widget with \c text and and an icon from \c iconFileName is used to represent the link in the list.
/*! If no category with this name exists, a new category is created.
  */
void AMSidebar::addLink(const QString& categoryName, QVariant linkContent, const QString& text, const QString& iconFileName, double weight) {

	AMSidebarDefaultSelector* selector = new AMSidebarDefaultSelector(text, iconFileName);
	this->addLink(categoryName, linkContent, selector, weight);
}

/// deletes the first link with a payload matching \c linkTarget
void AMSidebar::deleteLink(const QVariant& linkTarget) {

	QMutableHashIterator<QWidget*, AMSidebarLink> i(selector2link_);
	while(i.hasNext()) {
		AMSidebarLink& deleteMe = i.next().value();
		if(deleteMe.payload_ == linkTarget) {
			if(highlightedSelector_ == deleteMe.selector_)
				highlightedSelector_ = 0;
			name2category_[deleteMe.category_]->deleteLink(deleteMe);
			i.remove();
			break;
		}
	}
}


/// set the highlighted link (specified by the selector widget).
void AMSidebar::setHighlightedLink(QWidget* selector) {

	if(!selector2link_.contains(selector))
		return;

	// done already?
	if(highlightedSelector_ == selector)
		return;

	if(highlightedSelector_) {
		highlightedSelector_->setProperty("highlighted", false);
		highlightedSelector_->setStyleSheet(highlightedSelector_->styleSheet());
	}
	highlightedSelector_ = selector;
	highlightedSelector_->setProperty("highlighted", true);
	highlightedSelector_->setStyleSheet(highlightedSelector_->styleSheet());
}

/// set the highlighted link (specified by the link content).  This version is slower than setHighlightedLink(QWidget* selector).
/*! \todo optimize by indexing links also by linkContent */
void AMSidebar::setHighlightedLink(QVariant linkContent) {
	QHashIterator<QWidget*, AMSidebarLink> i(selector2link_);
	while(i.hasNext()) {
		const AMSidebarLink& found = i.next().value();
		if(found.payload_ == linkContent) {
			setHighlightedLink(found.selector_);
			break;
		}
	}
}


bool AMSidebar::eventFilter(QObject* sourceObject, QEvent* event) {

	QWidget* source = qobject_cast<QWidget*>(sourceObject);

	if(selector2link_.contains(source)) {

		switch(event->type()) {

		case QEvent::MouseButtonRelease:
			if(doubleClickInProgress_) {
				emit linkDoubleClicked(selector2link_[source].payload_);
			}
			else {
				emit linkClicked(selector2link_[source].payload_);
				setHighlightedLink(source);
			}

			break;


		case QEvent::MouseButtonDblClick:
			doubleClickInProgress_ = true;
			break;
		case QEvent::MouseButtonPress:
			doubleClickInProgress_ = false;
			break;


		default:
			break;
		}
	}
	return QFrame::eventFilter(source, event);
}
