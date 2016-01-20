#ifndef AMTOOLBUTTON_H
#define AMTOOLBUTTON_H

#include <QToolButton>

class AMToolButton : public QToolButton
{
	Q_OBJECT

public:
	/// Constructor.
	AMToolButton(QWidget *parent = 0);
	/// Destructor.
	virtual ~AMToolButton();

	/// The background color.
	QColor color() const { return color_; }

signals:
	/// Notifier that the background color has changed.
	void colorChanged(const QColor &newColor);

public slots:
	/// Sets the button background color.
	void setColor(const QColor &newColor);

	/// Sets the button background color to yellow.
	void setColorToYellow();
	/// Sets the button background color to red.
	void setColorToRed();
	/// Sets the button background color to green.
	void setColorToGreen();

protected:
	/// The paint event, reimplemented to apply a background color.
	virtual void paintEvent(QPaintEvent *event);

protected:
	/// The background color.
	QColor color_;
};

#endif // AMTOOLBUTTON_H
