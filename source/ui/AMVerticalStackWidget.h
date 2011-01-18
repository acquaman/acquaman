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


#ifndef AMVERTICALSTACKWIDGET_H
#define AMVERTICALSTACKWIDGET_H

#include <QFrame>
#include <QStandardItemModel>
#include <QSignalMapper>
#include "acquaman.h"
#include <QVBoxLayout>
#include <QSpacerItem>

/// The AMVerticalStackWidget class provides a column of widget items, that can be expanded or hidden.
class AMVerticalStackWidget : public QFrame
{
Q_OBJECT
public:
	/// Create a new vertical stack widget to hold a column of subwidgets.
	explicit AMVerticalStackWidget(QWidget *parent = 0);

	/// Number of widgets in the stack
	int count() const {
		return model_.rowCount();
	}

	/*
	/// If expanding = true, this widget will try to take up as much vertical space as available, and internal widgets will sit up at the top of it.
	void setExpanding(bool expanding);

	/// Returns whether expanding mode is on
	bool expanding() const { return spacer_; }
*/

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

	/// Overloaded to make sure that the horizontal size requested is as wide as all the inside widgets, even if they're collapsed and not currently shown in the layout.
	QSize sizeHint() const;


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
	// QSpacerItem* spacer_;


};


#endif // AMVERTICALSTACKWIDGET_H
