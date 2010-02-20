#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QHash>
#include <QVBoxLayout>


#define SIDEBAR_BG_COLOR 221, 227, 234
#define SIDEBAR_BORDER_COLOR 64, 64, 64

#define SIDEBAR_SELECTED_COLOR1 131, 137, 167
#define SIDEBAR_SELECTED_COLOR2 170, 176, 197
#define SIDEBAR_SELECTED_COLOR3 115, 122, 153

#define SIDEBAR_HIGHLIGHTED_COLOR1 69, 128, 200
#define SIDEBAR_HIGHLIGHTED_COLOR2 91, 146, 213
#define SIDEBAR_HIGHLIGHTED_COLOR3 22, 84, 170

#define SIDEBAR_MIN_HEIGHT 30
#define SIDEBAR_ICON_SIZE 22

#define SIDEBAR_BUTTON_STYLESHEET "QPushButton{ padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border: 0px solid black; background-color: rgb(221, 227, 234); font: 8pt \"Lucida Grande\"; color: rgb(0, 0, 0); }	QPushButton:hover {	border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); color: rgb(255, 255, 255); }"
#define SIDEBAR_SELECTEDBUTTON_STYLESHEET "QPushButton { padding-left: 10px; padding-right: 14px; padding-top: 0px; padding-bottom: 1px; text-align: left; border-width: 1px; border-style: solid;	font: 8pt \"Lucida Grande\"; color: rgb(0, 0, 0); border-color: rgb(115, 122, 153); border-top-color: rgb(131, 137, 167); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); color: rgb(255, 255, 255); }"

#define SIDEBAR_HEADER_STYLESHEET "QLabel { padding-left: 10px; padding-right: 14px; padding-top: 8px; padding-bottom: 4px; font: bold 8pt \"Lucida Grande\"; color: rgb(100, 109, 125); }"

// This class provides an iTunes/iPhoto style left sidebar, which supports category headers, icons, and (someday?) tiered links
// When a link is clicked, it emits the linkActivated(QWidget*) signal which you can use to notify the app to activate that widget.
// The last-clicked link is also highlighted.


class Sidebar : public QFrame {
	
	Q_OBJECT
	
public:
	Sidebar(QWidget* parent = 0) : QFrame(parent) {
		
		topLayout_ = new QVBoxLayout(this);
		topLayout_->setSpacing(12);
		topLayout_->setContentsMargins(0, 0, 0, 0);
		
		
		// Add an expanding space at the bottom...
		topLayout_->addStretch();
		
		setStyleSheet("Sidebar { background-color: rgb(221, 227, 234);   border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64);    }");
		
		selectedButton_ = 0;
	}
	
	// add a Category (text header and group for links) at category position 'position' (neg. for at end)
	void addCategory(const QString& categoryName, int position = -1) {
		
		// If this category already exists...
		if (catName2Layout_.contains(categoryName))
			return;
		
		// Create the layout for this category:
		QVBoxLayout* layout = new QVBoxLayout();
		layout->setSpacing(0);
		
		// Title this category:
		QLabel* label = new QLabel(categoryName.toUpper());
		label->setStyleSheet(SIDEBAR_HEADER_STYLESHEET);
		layout->addWidget(label);
		
		// Leave the spacer at the bottom
		if(position < 0)
			position = topLayout_->count() - 1;
			
		topLayout_->insertLayout(position, layout);
		
		catName2Layout_.insert(categoryName, layout);	
		
	}
	
	// add a link (text "linkName", follow-content linkContent) under categoryName at position, with icon.
	void addLink(const QString& linkName, QWidget* linkContent, const QString& categoryName, int position = -1, const QIcon& icon = QIcon()) {
		
		if( !catName2Layout_.contains(categoryName) )
			addCategory(categoryName);
			
		QVBoxLayout* layout = catName2Layout_[categoryName];
		
		QPushButton* button = new QPushButton(icon, linkName);
		button->setContentsMargins(6, 6, 6, 6);
		button->setMinimumHeight(SIDEBAR_MIN_HEIGHT);
		button->setIconSize(QSize(SIDEBAR_ICON_SIZE, SIDEBAR_ICON_SIZE));
		button->setStyleSheet( SIDEBAR_BUTTON_STYLESHEET );
		
		layout->insertWidget(position, button);
		
		connect(button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
		
		linkContent2Button_.insert(linkContent, button);
		button2LinkContent_.insert(button, linkContent);
		
		
	}
	
	// remove a link:
	void removeLink(QWidget* linkContent) {
	
		if( linkContent2Button_.contains( linkContent ) ) {
			
			linkContent2Button_[linkContent]->deleteLater();
			button2LinkContent_.remove( linkContent2Button_[linkContent] );
			linkContent2Button_.remove(linkContent);
			
		}	
		
		
	}
	
	// 
	// void addSubLink(QWidget* linkParent, const QString& linkName, QWidget* linkContent, int index = -1, const QIcon& icon = QIcon());

protected slots:

	void onButtonClicked() {
		
		// clear the old selected button:
		if(selectedButton_)
			selectedButton_->setStyleSheet( SIDEBAR_BUTTON_STYLESHEET );
		
		// find out who was clicked:
		QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
		if(clickedButton) {
		
			if( button2LinkContent_.contains(clickedButton) )
				emit linkActivated( button2LinkContent_[clickedButton] );
	
			// decorate the newly selected button:
			selectedButton_ = clickedButton;
			selectedButton_->setStyleSheet( SIDEBAR_SELECTEDBUTTON_STYLESHEET );
		}
		
	}
		
signals:
	void linkActivated(QWidget* activatedLink);
	
	
protected:
	
	QVBoxLayout* topLayout_;
	
	QHash<QString, QVBoxLayout*> catName2Layout_;
	
	QHash<QWidget*, QPushButton*> linkContent2Button_;
	QHash<QPushButton*, QWidget*> button2LinkContent_;
	
	QPushButton* selectedButton_;
	
	
	
	
};

#endif