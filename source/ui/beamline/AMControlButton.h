#ifndef AMCONTROLBUTTON_H
#define AMCONTROLBUTTON_H

#include <QToolButton>
class AMControl;


class AMControlButton : public QToolButton
{
Q_OBJECT
public:
	AMControlButton(AMControl *control, QWidget *parent = 0);

	int downValue() const;
	int upValue() const;

public slots:
	void setDownValue(double downValue);
	void setUpValue(double upValue);

	void overrideText(const QString& text);
	void setCheckable(bool);

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	void onValueChanged(double newVal);
	void onClicked();
	void onToggled(bool toggled);

protected:
	AMControl *control_;
	double downValue_;
	double upValue_;
	bool programaticToggle_;
};


#endif // AMCONTROLBUTTON_H
