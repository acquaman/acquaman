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


#ifndef AMCLOSEITEMDELEGATE_H
#define AMCLOSEITEMDELEGATE_H

#include <QStyledItemDelegate>

class AMCloseItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:

	/// Describes the behaviour when close buttons are enabled, and an item's close button is clicked.
	enum CloseButtonAction {
		SignalOnly, /// < only the closeButtonClicked() signal is emitted
		RemoveFromModel /// < the closeButtonClicked() signal is emitted, but the item's row is also removed from the model
	};

	/// Default constructor. Close buttons are disabled, and the CloseButtonAction is SignalOnly by default.
	explicit AMCloseItemDelegate(QObject *parent = 0);
	/// Destructor.
	virtual ~AMCloseItemDelegate();

	/// Re-implemented from QStyledItemDelegate to draw the close button if closeButtonEnabled(). Drawing it is done by the protected drawCloseButton() function, which can be called by subclasses in their own paint functions.
	virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Re-implemented to ask for a little extra space, when the close buttons are enabled.
	virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Re-implemented from QStyledItemDelegate to check if the close button has been clicked. If closeButtonsEnabled() and one of the close buttons is clicked, it will emit closeButtonClicked() with the index of the item.
	bool editorEvent ( QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index );


	/// indicates whether close buttons are displayed for each item
	bool closeButtonsEnabled() const { return closeButtonEnabled_; }
	/// indicates whether items are immediately removed and deleted from the model when a close button is clicked
	CloseButtonAction closeButtonAction() const { return closeButtonAction_; }


signals:
	void closeButtonClicked(const QModelIndex& itemIndex);

public slots:
	void setCloseButtonsEnabled(bool closeButtonsOn = true) {
		if( (closeButtonEnabled_ = closeButtonsOn) )
			closeButton_ = QPixmap(":/closeButton.png");
		else
			closeButton_ = QPixmap();
	}
	/// This controls the behaviour when the close buttons are enabled, and the user clicks the close button on an item.  If the \c closeButtonAction is AMDetailedItemDelegate::SignalOnly, then only the closeButtonClicked() signal is emitted with the index of the item.  If the \c closeButtonAction is AMDetailedItemDelegate::RemoveFromModel, then the item will be instantly removed from the model as well.
	void setCloseButtonAction(int closeButtonAction = SignalOnly) {
		closeButtonAction_ = (CloseButtonAction)closeButtonAction;
	}

protected:

	/// Implements drawing of the close button. Can be re-implemented or called by subclasses. It's important to set the closeButtonRect_ as the active area of the button wherever you place it. (in item-local coordinates, referenced to the topLeft() corner of the item).
	virtual void drawCloseButton(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	/// Icon for the close button
	QPixmap closeButton_;
	/// Whether close buttons are currently displayed
	bool closeButtonEnabled_;
	/// holds the rectangle occupied by the close button, in "local" (referenced to item rect's topLeft()) coordinates
	mutable QRect closeButtonRect_;


	CloseButtonAction closeButtonAction_;

	/// Tracks where the mouse click started
	QPoint mouseDownPosition_;
};

#endif // AMCLOSEITEMDELEGATE_H
