/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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
#include "actions/AMBeamlineParallelActionsList.h"

class QLabel;
class QDoubleSpinBox;
class QPushButton;
class QMovie;
class QPixmap;


/// This helper class for AMBasicControlEditor provides a dialog box to get new setpoints. You should never need to use it directly.
class AMControlEditorStyledInputDialog : public QDialog {
	Q_OBJECT
public:
	AMControlEditorStyledInputDialog( QWidget * parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

public slots:
	void setDoubleValue(double d);
	void setDoubleMaximum(double d);
	void setDoubleMinimum(double d);
	void setDoubleDecimals(int d);
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


/// This widget can be used to view the feedback position and move the setpoint of any AMControl, in a compact space-efficient way.  Normally, the control's value(), units(), and isMoving() status are shown. When a user clicks on the widget, a popup dialog is opened under their mouse; they can enter a new setpoint and hit enter to move the control.
class AMControlEditor : public QFrame
{
Q_OBJECT
public:
	/// Constructor: requires a pointer to the \c control to show, or 0.
	AMControlEditor(AMControl *control, QWidget *parent = 0);

	/// Returns true if the widget is in readOnly() mode, and cannot be used to move() the control.
	bool readOnly() const { return readOnly_; }

signals:
	/// Emitted when the widget is clicked to bring up the setpoint editor dialog.
	void clicked();

public slots:
	/// If you want to prevent users from editing a control using this widget, you can set it to readOnly().
	void setReadOnly(bool readOnly);

protected slots:
	/// Called to review the color of the unit box and any status icons that need to be shown. Called on the control's connected(), initialized(), alarmChanged() signals.
	/*! It's OK to call this when control_ == 0; all other onChanged() slots should only be called under signals from a valid control_*/
	void reviewControlState();

	/// Called when the control's value changes; updates the valueLabel_.
	void onValueChanged(double newVal);
	/// Called when the control's units change; updates the value suffix.
	void onUnitsChanged(const QString& units);

	/// Called when the control's isMoving() state changes; shows the moving icon.
	void onMotion(bool moving);
	/// Called when the control's enum information changes. We show or hide the enum button, and configure the entries in it.
	void onEnumChanges();
	/// Called when the user chooses an enum value from the enumButton_ popup. Calls onNewSetpointChosen with the value for that enum.
	void onEnumValueChosen(QAction* action);

	/// Called to bring up the setpoint editor dialog.
	void onEditStart();
	/// Called when the setpoint editor dialog is accepted with a new \c value. The default implementation simply calls move() on the control.  You can re-implement this in a subclass if you want to have different behaviour (for ex: using an AMAction to do the move, etc.)
	virtual void onNewSetpointChosen(double value);

protected:
	AMControl *control_;
	QLabel *valueLabel_;
	QFrame* statusFrame_;
	QToolButton* enumButton_;
	QMenu* enumMenu_;
	AMControlEditorStyledInputDialog* dialog_;

	static QMovie* movingIcon_;
	static QPixmap* invalidIcon_, *minorIcon_, *majorIcon_, *lockedIcon_;

	QLabel* movingLabel_, *invalidLabel_, *minorLabel_, *majorLabel_, *lockedLabel_;

	bool readOnly_;

	/// Re-implemented to emit clicked() when clicked anywhere. (Normally, QFrames don't act like buttons.)
	void mouseReleaseEvent ( QMouseEvent *event );

};

class AMExtendedControlEditorStyledInputDialog : public QDialog {
	Q_OBJECT
public:
	AMExtendedControlEditorStyledInputDialog( QStringList enumNames = QStringList(), QWidget *parent = 0, Qt::WindowFlags flags = (Qt::Dialog | Qt::FramelessWindowHint) );

	double setpoint() const;

public slots:
	void setDoubleValue(double d);
	void setDoubleMaximum(double d);
	void setDoubleMinimum(double d);
	void setDoubleDecimals(int d);
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

class AMExtendedControlEditor : public QGroupBox
{
Q_OBJECT
public:
	explicit AMExtendedControlEditor(AMControl* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);

	double setpoint() const;
	AMControl* control() const;

	bool setControlFormat(const QChar& format, int precision);

signals:
	void moveRequested(double);
	void setpointRequested(double);
	void clicked();

public slots:
	void setReadOnly(bool readOnly);
	void setNoUnitsBox(bool noUnitsBox);
	void overrideTitle(const QString& title);

	void setSetpoint(double newSetpoint);

	/* NTBA April 11, 2011 David Chevrier
	   Why is this here?
	*/
	QSize sizeHint() const;

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	void onValueChanged(double newVal);
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);

	void onEditStart();
	void onNewSetpoint(double newVal);

	void onStatusValueChanged(double newVal);

protected:
	void mouseReleaseEvent ( QMouseEvent * event );

protected:
	AMControl* control_;
	AMControl* statusTagControl_;
	bool readOnly_;
	bool configureOnly_;
	bool connectedOnce_;
	bool newValueOnce_;
	QChar format_;
	int precision_;

	QLabel* valueLabel_;
	QLabel* unitsLabel_;
	QLabel* statusLabel_;
	AMExtendedControlEditorStyledInputDialog* dialog_;
};

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


#endif // AM_CONTROLEDITOR_H
