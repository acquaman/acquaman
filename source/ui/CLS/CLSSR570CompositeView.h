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


#ifndef CLSSR570COMPOSITEVIEW_H
#define CLSSR570COMPOSITEVIEW_H

#include <QWidget>

#include <QComboBox>
#include <QToolButton>

#include "beamline/CLS/CLSSR570.h"

class CLSSR570CompositeView : public QWidget
{
	Q_OBJECT

public:
	/// Simple enum holding whether the view should be either basic or advanced looking.
	enum ViewMode { Basic = 0, Advanced = 1 };

	/// Constructor.  Requires two valid SR570 pointers.
 	virtual ~CLSSR570CompositeView();
	CLSSR570CompositeView(CLSSR570 *sr1, CLSSR570 *sr2, QWidget *parent = 0);

	/// Returns the pointer to the first SR570 this view is visualizing.
	CLSSR570 *firstSR570() const { return firstSR570_; }
	/// Returns the pointer to the second SR570 this view is visualizing.
	CLSSR570 *secondSR570() const { return secondSR570_; }

	/// Returns the current view mode.
	CLSSR570CompositeView::ViewMode viewMode() const { return mode_; }
	/// Switches the view from basic to advanced or advanced to basic.
	void setViewMode(CLSSR570CompositeView::ViewMode mode);

signals:
	/// Notifier that the view has changed.  Passes the new view.
	void viewModeChanged(CLSSR570CompositeView::ViewMode);

public slots:

protected slots:
	/// Handles passing changes in the value combo box to the ion chamber.
	void onValueComboBoxChanged(int index);
	/// Handles passing changes in the units combo box to the ion chamber.
	void onUnitsComboBoxChanged(int index);

	/// Handles setting the value combo box when the ion chamber is changed from elsewhere.
	void onValueChanged(int value);
	/// Handles setting the units combo box when the ion chamber is changed from elsewhere.
	void onUnitsChanged(QString units);

	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);

protected:
	/// The pointer to the first SR570 this view manages.
	CLSSR570 *firstSR570_;
	/// The pointer to the second SR570 this view manages.
	CLSSR570 *secondSR570_;

	/// Combo box holding the value for the sensitivity.
	QComboBox *value_;
	/// Combo box holding the units for the sensitivity.
	QComboBox *units_;

	/// The tool button for the minus button.
	QToolButton *minus_;
	/// The tool button for the plus button.
	QToolButton *plus_;

	/// Flag holding whether or not the view is basic or advanced.
	ViewMode mode_;
};

#endif // CLSSR570COMPOSITEVIEW_H
