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

public slots:
	/// Sets the button background color.
	void setColor(const QColor &newColor);

	/// Sets the button background color to yellow.
	void setColorToYellow();
	/// Sets the button background color to red.
	void setColorToRed();
	/// Sets the button background color to green.
	void setColorToGreen();
};

#endif // AMTOOLBUTTON_H
