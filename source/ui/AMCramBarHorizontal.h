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
	void onScrollWidgetResized(const QSize&) {
		scrollArea_->updateGeometry();
		//or, could use: outerLayout_->activate();
	}

	/// called when the scroll area itself is resized:
	void onScrollAreaResized(const QSize&);

protected:
	AMScrollArea* scrollArea_;
	QToolButton* scrollLeftButton_, *scrollRightButton_;
	AMSizeSignallingWidget* scrollWidget_;
	QHBoxLayout* innerLayout_, *outerLayout_;

};

#endif // AMCRAMBARHORIZONTAL_H
