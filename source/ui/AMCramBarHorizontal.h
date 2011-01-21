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


#ifndef AMCRAMBARHORIZONTAL_H
#define AMCRAMBARHORIZONTAL_H

#include <QWidget>
#include <QScrollArea>
#include <QResizeEvent>
#include <QToolButton>
#include <QHBoxLayout>



/// A replacement for Qt's QScrollArea. This version's QWidget::sizeHint() asks the internal widget for its own sizeHint(), which is usually what makes the most sense. (In other words, the scroll area will request to be the size of its internal widget, when possible.  To make full use of this, you need a way of being informed of changes in the size of that internal widget; see AMSizeSignallingWidget.)
class AMScrollArea : public QScrollArea {
	Q_OBJECT
public:
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

/// This widget can be useful as a container QWidget, which provides a resized() signal whenever it is resized.
class AMSizeSignallingWidget : public QWidget {
	Q_OBJECT

public:
	explicit AMSizeSignallingWidget(QWidget* parent = 0)
		: QWidget(parent) {}

signals:
	void resized(const QSize& newSize);

protected:
	void resizeEvent(QResizeEvent *event) {
		emit resized(event->size());
		QWidget::resizeEvent(event);
	}
};

/// This class provides a container widget with a horizontal layout. The contents are placed inside a scroll area so that they can extend "off-widget" if the container is too small. Scroll buttons appear at both ends of the bar whenever the extra room is required; they disappear when not needed.
class AMCramBarHorizontal : public QWidget
{
Q_OBJECT
public:
	explicit AMCramBarHorizontal(QWidget *parent = 0);

	void addWidget(QWidget* widget) {
		innerLayout_->addWidget(widget);
	}

	QHBoxLayout* getLayout() {
		return innerLayout_;
	}

signals:

public slots:


protected slots:
	/// called when either scroll button (for the channel button scroll area) is clicked
	void onScrollButtonClicked();

	/// called when the contents of the scroll widget (ie: the set of channel buttons) changes size
	void onScrollWidgetResized(const QSize&);

	/// called when the scroll area itself is resized:
	void onScrollAreaResized(const QSize&) { onScrollAreaResized(); }
	void onScrollAreaResized();

protected:
	AMScrollArea* scrollArea_;
	QToolButton* scrollLeftButton_, *scrollRightButton_;
	AMSizeSignallingWidget* scrollWidget_;
	QHBoxLayout* innerLayout_, *outerLayout_;
	bool scrollButtonsOn_;

};

#endif // AMCRAMBARHORIZONTAL_H
