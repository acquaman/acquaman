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


#ifndef AM_CONTROLEDITOR_H
#define AM_CONTROLEDITOR_H

#include <QFrame>
#include <QDialog>
#include <QGroupBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>

#include "beamline/AMControl.h"
#include "beamline/AMBeamlineParallelActionsList.h"

class QLabel;
class QDoubleSpinBox;
class QPushButton;

// TODO: finish this for numeric controls only... and then generalize this and Control to: Numeric, Item, String.

// Helper class: the dialog box to get new setpoints:
class AMBasicControlEditorStyledInputDialog : public QDialog {
	Q_OBJECT
public:
	AMBasicControlEditorStyledInputDialog( QWidget * parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

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


class AMBasicControlEditor : public QFrame
{
Q_OBJECT
public:
	explicit AMBasicControlEditor(AMControl *control, QWidget *parent = 0);

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
	AMBasicControlEditorStyledInputDialog* dialog_;

	void mouseReleaseEvent ( QMouseEvent *event );

};

class AMControlEditorStyledInputDialog : public QDialog {
	Q_OBJECT
public:
	AMControlEditorStyledInputDialog( QStringList enumNames = QStringList(), QWidget *parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

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

class AMControlEditor : public QGroupBox
{
Q_OBJECT
public:
	explicit AMControlEditor(AMControl* control, AMControl* statusTagControl = NULL, bool readOnly = false, QWidget *parent = 0);

signals:
	void moveRequested(double);
	void clicked();

public slots:
	void setReadOnly(bool readOnly);
	void setNoUnitsBox(bool noUnitsBox);
	void overrideTitle(const QString& title);

	QSize sizeHint() const;

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	void onValueChanged(double newVal);
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);

	void onEditStart();

	void onStatusValueChanged(double newVal);

protected:
//	QSize sizeHint() const;
	void mouseReleaseEvent ( QMouseEvent * event );

protected:
	AMControl* control_;
	AMControl* statusTagControl_;
	bool readOnly_;
	QLabel* valueLabel_;
	QLabel* unitsLabel_;
	QLabel* statusLabel_;
	//QLabel* nameLabel_;
	AMControlEditorStyledInputDialog* dialog_;
};

//class AMControlButton : public QPushButton
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

// Starts the first action in the actionsList as long as it isn't already finished (that would put us in a problematic state)
class AMBeamlineActionsListButton : public QToolButton
{
Q_OBJECT
public:
	AMBeamlineActionsListButton(AMBeamlineParallelActionsList *actionsList, QWidget *parent = 0);

public slots:
	void overrideText(const QString& text);

protected slots:
	void onClicked();

protected:
	AMBeamlineParallelActionsList *actionsList_;
};

#endif // AM_CONTROLEDITOR_H
