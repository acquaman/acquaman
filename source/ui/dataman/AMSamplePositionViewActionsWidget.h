/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H
#define AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H

#include <QFrame>

namespace Ui {
	class AMSamplePositionViewActionsWidget;
}

/// This widget adds a "Mark", "Move To", and "Remove" button when editing sample position items; it's used by AMSamplePlateView.
class AMSamplePositionViewActionsWidget : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  \c row is the row index that we are acting as the editor for.
	explicit AMSamplePositionViewActionsWidget(int row, QWidget *parent = 0);
	~AMSamplePositionViewActionsWidget();

signals:
	/// Emitted with the row number when the 'Mark' button is pressed
	void rowMarkPressed(int row);
	/// Emitted with the row number when the 'Move To' button is pressed
	void rowMoveToPressed(int row);
	/// Emitted with the row number when the "Remove" button is pressed
	void rowRemovePressed(int row);

protected:
	/// The row that this editor is being used on. (Since we only handle list models, this is sufficient)
	int row_;

protected slots:
	void onMarkButtonPressed() { emit rowMarkPressed(row_); }
	void onMoveToButtonPressed() { emit rowMoveToPressed(row_); }
	void onRemoveButtonPressed() { emit rowRemovePressed(row_); }

private:
	Ui::AMSamplePositionViewActionsWidget *ui;
};

#endif // AMSAMPLEPOSITIONVIEWACTIONSWIDGET_H
