#ifndef AMVERTICALSTACKWIDGET_H
#define AMVERTICALSTACKWIDGET_H

#include <QFrame>
#include <QStandardItemModel>
#include <QSignalMapper>
#include "acquaman.h"
#include <QVBoxLayout>

#define AMVERTICALSTACKWIDGET_STYLE_SHEET "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(233, 233, 233, 255), stop:0.502513 rgba(199, 198, 198, 255), stop:1 rgba(163, 162, 162, 255)); border: 1px solid black; border-top-color: rgb(180, 180, 180); border-bottom-color: rgb(121, 121, 121); border-left-color: rgb(180, 180, 180); border-right-color: rgb(180, 180, 180);"

/// The AMVerticalStackWidget class provides a column of widget items, that can be expanded or hidden.
class AMVerticalStackWidget : public QFrame
{
Q_OBJECT
public:
	explicit AMVerticalStackWidget(QWidget *parent = 0);

	/// Number of widgets in the stack
	int count() const {
		return model_.rowCount();
	}

	/// Add a widget at the bottom of the stack. Widgets are initially shown expanded.  The AMVerticalStackWidget takes ownership of the widget.
	void addItem(QWidget* widget, const QString& text, bool collapsable = true) {
		insertItem(-1, widget, text, collapsable);
	}

	/// Insert a widget at a specific index in the stack. Inserting at \c index = -1 is equivalent to appending to the end. The AMVerticalStackWidget takes ownership of the widget.
	void insertItem(int index, QWidget* widget, const QString& text, bool collapsable = true);



	/// Return the index of a widget in the stack. (-1 if not found)
	int indexOf(QWidget* widget) const;
	/// Return the widget at a specific index, or 0 for invalid indexes
	QWidget* widget(int index) const;


	/// Remove a widget, and delete it
	void removeItem(int index) {
		delete takeItem(index);
	}

	/// Remove a widget and return it. Ownership becomes the responsibility of the caller.
	QWidget* takeItem(int index);

	/// Is this item collapsable?
	bool itemCollapsable(int index) const;
	/// Is this item collapsed?
	bool itemIsCollapsed(int index) const;

	/// Set the heading text for a widget
	void setItemText(int index, const QString& text);


signals:
	/// Emitted when a widget is expanded:
	void expanded(QWidget*);
	/// Emitted when the widget at this index is expanded
	void expanded(int);
	/// Emitted when a widget is collapsed
	void collapsed(QWidget*);
	/// Emitted when a widget at this index is collapsed
	void collapsed(int);


public slots:
	/// Expand a given widget
	void expandItem(int index);
	/// Collapse a given widget
	void collapseItem(int index);

protected slots:
	/// Called when any of the header buttons is clicked. We might need to expand or collapse a widget section.
	void onHeaderButtonClicked();

protected:
	/// Internally, we use a QStandardItemModel to store the widgets':
	/*! AMPointerRole: the actual QWidget* pointer
		Qt::DisplayRole: Title String
		Qt::DecorationRole: Title Icon (unimplemented for now)
		Qt::ToolTipRole: Title tooltip (unimplemented for now)
		Qt::CheckStateRole: Checked indicates collapsed. Initially false.
		[item flag] Qt::ItemIsUserCheckable: indicates widget can be collapsed
		AM::WidgetRole: a pointer to the QToolButton used as a header widget

		Currently, the model is for internal use only.  This class doesn't yet respond to the dataChanged() signal, so modifying the model will not trigger changes in the display of the widget.  This can be implemented if desired.
		*/
	QStandardItemModel model_;
	// QSignalMapper* headerButtonMapper_;

	QVBoxLayout* vl_;

};


#endif // AMVERTICALSTACKWIDGET_H
