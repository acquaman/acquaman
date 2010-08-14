#ifndef AMMAINWINDOW_H
#define AMMAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QHash>
#include <QMultiHash>
#include <QStackedWidget>
#include <QEvent>
#include "ui/AMSidebar.h"



/// This UI class manages a set of "window panes", which can either be docked and selected using an iTunes-style sidebar, or un-docked to float as independent windows. When an un-docked pane is clicked in the sidebar or closed, it is re-docked in the main window. Finally, a layout is available to add a custom bottom widget underneath the sidebar and main area.
class AMMainWindow : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
	explicit AMMainWindow(QWidget *parent = 0);

	/// AMMainWindow takes ownership for all of its pane widgets (whether docked or undocked), and deletes them in the destructor.
	virtual ~AMMainWindow();

	/// Add a custom widget below the sidebar and main area
	void addBottomWidget(QWidget* bottomWidget) {
		vlayout_->addWidget(bottomWidget);
	}
	/// Add a custom widget above the sidebar and main area
	void addTopWidget(QWidget* topWidget) {
		vlayout_->insertWidget(0, topWidget);
	}

	/// Add a new \c pane to manage.  It will show up in the sidebar under category \c categoryName, at the given \c weight, with a \c title and an icon from \c iconFileName.  Returns a pointer to the sidebar item in the sidebar.
	QStandardItem* addPane(QWidget* pane, const QString& categoryName, const QString& title, const QString& iconFileName, double weight = 0);


	/// Remove and delete a pane widget (whether docked or undocked)
	void deletePane(QWidget* pane);

	/// Remove a pane widget but do not delete it.  Ownership is now the responsibility of the caller. The pane becomes a top-level window.
	void removePane(QWidget* pane);

	/// Access the main window's sidebar object
	AMSidebar* sidebar() {
		return sidebar_;
	}


public slots:
	/// move a pane from inside the main window to a separate window.
	void undock(QWidget* pane);

	/// return a \c pane that was undocked back to the main window.  Does not set this pane as the current widget.
	void dock(QWidget* pane);

	// void goToPane(const QVariant& paneVariant);



protected slots:
	/// Called when a link in the sidebar is clicked.  This should set the linked pane as the current widget, re-capturing it if necessary.
	void onSidebarLinkClicked(const QVariant& linkContent);


	/// Called when a link in the sidebar is double-clicked. This should undock the linked pane and set it up as a new window.
	void onSidebarLinkDoubleClicked(const QVariant& linkContent);

	/// We intercept and forward the currentChanged(int) signal from the QStackedWidget, to keep the sidebar's highlighted link consistent with the current widget.
	void onFwdCurrentWidgetChanged(int currentIndex);

protected:
	QHash<QWidget*, bool> pane2isDocked_;
	QMultiHash<QWidget*, QStandardItem*> pane2sidebarItems_;

	QStackedWidget* stackWidget_;

	AMSidebar* sidebar_;

	QVBoxLayout* vlayout_;

	QWidget* previousPane_;


	/// if an undocked widget is about to be closed, we need to intercept that event and capture (dock) it back to the main window insead.
	bool eventFilter(QObject* sourceObject, QEvent* event);
};

#endif // AMMAINWINDOW_H
