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


#ifndef ACQMAN_NUMERICCONTROL_H
#define ACQMAN_NUMERICCONTROL_H

#include <QFrame>
#include <QDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "beamline/AMControl.h"

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
	explicit NumericControl(AMControl *control, QWidget *parent = 0);

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
	AMControl *control_;
	QLabel *valueLabel_;
	QLabel *unitsLabel_;
	StyledInputDialog* dialog_;
	
	void mouseReleaseEvent ( QMouseEvent *event );

};

class StyledControlInputDialog : public QDialog {
	Q_OBJECT
public:
	StyledControlInputDialog( QStringList enumNames = QStringList(), QWidget *parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

public slots:
	void setDoubleValue(double d);
	void setDoubleMaximum(double d);
	void setDoubleMinimum(double d);
	void setDoubleDecimals(double d);
	void setLabelText(const QString& s);
	void setEnumNames(const QStringList &sl);
	void setSuffix(const QString& s);

signals:
	void doubleValueSelected(double);

protected slots:
	void onAccepted();

protected:
	void resizeEvent(QResizeEvent */* event */);
	void showEvent ( QShowEvent *event );

protected:
	QStringList enumNames_;
	bool isEnum_;
	QDoubleSpinBox *spinBox_;
	QComboBox *comboBox_;
	QLabel *label_;
	QPushButton *okButton_, *cancelButton_;
	QVBoxLayout *vl_;
	QHBoxLayout *hl_;
};

class AMControlEdit : public QGroupBox
{
Q_OBJECT
public:
	explicit AMControlEdit(AMControl* control, bool readOnly = false, QWidget *parent = 0);

signals:
	void moveRequested(double);
	void clicked();

public slots:
	void setReadOnly(bool readOnly);

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	void onValueChanged(double newVal);
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);

	void onEditStart();

protected:
	QSize sizeHint() const;
	void mouseReleaseEvent ( QMouseEvent * event );

protected:
	AMControl* control_;
	bool readOnly_;
	QLabel* valueLabel_;
	QLabel* unitsLabel_;
	//QLabel* nameLabel_;
	StyledControlInputDialog* dialog_;
};

#endif // NUMERICCONTROL_H
