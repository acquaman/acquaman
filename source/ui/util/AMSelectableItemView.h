/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMSELECTABLEITEMVIEW_H
#define AMSELECTABLEITEMVIEW_H

#include <QWidget>

#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>

/// Base class for a selectable item (either an emission line or absorption edge).  Provides a simple base class for simple items that are selectable.
class AMSelectableItemView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds the typical view for the selectable item.
 	virtual ~AMSelectableItemView();
	explicit AMSelectableItemView(QWidget *parent = 0);
	/// Constructor.  Passes in the text and default checked state of the view.
	AMSelectableItemView(const QString &text, bool checked = false, QWidget *parent = 0);

	/// Returns the current state of the check box used for selecting.
	bool isItemSelected() const { return checkBox_->isChecked(); }

signals:
	/// Notifier that the item has been selected or not.  Passes the state.
	void itemSelected(bool);

public slots:
	/// Sets the text for the name label.
	void setText(const QString &text);
	/// Sets the state of the check box.
	void setSelected(bool isSelected);

protected:
	/// The check box used for selecting the item.
	QCheckBox *checkBox_;
	/// The layout holding all the widgets.
	QHBoxLayout *itemLayout_;
};

#endif // AMSELECTABLEITEMVIEW_H
