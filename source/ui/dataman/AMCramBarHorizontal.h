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


#ifndef AMCRAMBARHORIZONTAL_H
#define AMCRAMBARHORIZONTAL_H

#include <QWidget>
#include <QScrollArea>
#include <QResizeEvent>
#include <QToolButton>
#include <QHBoxLayout>

class AMDeferredFunctionCall;


/// A replacement for Qt's QScrollArea. This version's QWidget::sizeHint() asks the internal widget for its own sizeHint(), which is usually what makes the most sense. (In other words, the scroll area will request to be the size of its internal widget, when possible.)
class AMScrollArea : public QScrollArea {
	Q_OBJECT
public:
 	virtual ~AMScrollArea();
	explicit AMScrollArea(QWidget* parent = 0)
		: QScrollArea(parent) {}

	virtual QSize sizeHint() const {
		if(widget()) {
			return widget()->sizeHint();
		}
		else
			return QSize();
	}

	virtual QSize minimumSizeHint() const {
		return QSize(0,0);
	}

signals:
	void resized(const QSize& newSize);

protected:
	void resizeEvent(QResizeEvent *e) {
		emit resized(e->size());
		QScrollArea::resizeEvent(e);
	}
};

/// This class provides a container widget with a horizontal layout. The contents are placed inside a scroll area so that they can extend "off-widget" if the container is too small. Scroll buttons appear at both ends of the bar whenever the extra room is required, they disappear when not needed.
class AMCramBarHorizontal : public QWidget
{
Q_OBJECT
public:
 	virtual ~AMCramBarHorizontal();
	explicit AMCramBarHorizontal(QWidget *parent = 0);

	void addWidget(QWidget* widget) {
		innerLayout_->addWidget(widget);
	}

	void insertWidget(int atIndex, QWidget* widget) {
		innerLayout_->insertWidget(atIndex, widget);
	}

signals:

public slots:


protected slots:
	/// called when either scroll button (for the channel button scroll area) is clicked
	void onScrollButtonClicked();

	void reviewScrollButtonsRequired();

	/// Catches layout request events on the inner widget (the widget inside the scroll area).  This will detect changes in how much size the scroll area / inner widget needs.  Also catches layout requests on OUR parent widget (if we have one)
	bool eventFilter(QObject* source, QEvent* event);

protected:
	AMScrollArea* scrollArea_;
	QToolButton* scrollLeftButton_, *scrollRightButton_;
	QWidget* scrollWidget_;
	QHBoxLayout* innerLayout_, *outerLayout_;
	bool scrollButtonsOn_;

	QWidget* parentWidget_;
	AMDeferredFunctionCall* checkIfScrollButtonsRequired_;

	/// Catches layout request events for ourself (ie: when the total size we have available has changed, or when the sizes of things inside our main layout changes). Turns on the scroll bar arrows when required. (This will cause a subsequent layout request event... but that's okay.)
	bool event(QEvent* event);

};

#endif // AMCRAMBARHORIZONTAL_H
