#ifndef ACQMAN_NUMERICCONTROL_H
#define ACQMAN_NUMERICCONTROL_H

#include <QFrame>
#include <QDialog>

#include "beamline/Control.h"

class QLabel;
class QDoubleSpinBox;
class QPushButton;

// TODO: finish this for numeric controls only... and then generalize this and Control to: Numeric, Item, String.

// Helper class: the dialog box to get new setpoints:
class StyledInputDialog : public QDialog {
	Q_OBJECT
public:
	StyledInputDialog( QWidget * parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

public slots:
	void setDoubleValue(double d);
	void setDoubleMaximum(double d);
	void setDoubleMinimum(double d);
	void setDoubleDecimals(double d);
	void setLabelText(const QString& s);
	void setSuffix(const QString& s);

signals:
	void doubleValueSelected(double);
protected:
	void resizeEvent(QResizeEvent * /* event */);
	void showEvent ( QShowEvent * event );

	QDoubleSpinBox* spinBox_;
	QLabel* label_;
	QPushButton* okButton_, *cancelButton_;

protected slots:
	void onAccepted();

};
// TODO: moving indicator
// TODO: move dialog

class NumericControl : public QFrame
{
Q_OBJECT
public:
        explicit NumericControl(AMControl* control, QWidget *parent = 0);

signals:
	void moveRequested(double);
	void clicked();

public slots:

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	void onValueChanged(double newVal);
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);
	
	void onEditStart();

protected:
        AMControl* control_;
	QLabel* valueLabel_;
	QLabel* unitsLabel_;
	StyledInputDialog* dialog_;
	
	void mouseReleaseEvent ( QMouseEvent * event );

};

#endif // NUMERICCONTROL_H
