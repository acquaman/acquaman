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

#ifndef AMEXTENDEDCONTROLEDITOR_H
#define AMEXTENDEDCONTROLEDITOR_H

#include <QGroupBox>
#include <QDialog>

class QDoubleSpinBox;
class QLabel;
class QPushButton;
class QComboBox;
class QVBoxLayout;
class QHBoxLayout;

class AMControl;

class AMExtendedControlEditorStyledInputDialog : public QDialog {
	Q_OBJECT
public:
 	virtual ~AMExtendedControlEditorStyledInputDialog();
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
	/// Constructor.
	explicit AMExtendedControlEditor(AMControl* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMExtendedControlEditor();

	double setpoint() const;
	AMControl* control() const;

	bool setControlFormat(const QChar& format, int precision);
	void setPrecision(int precision);
	void setRange(double maxValue, double minValue);
	void hideBorder();

	/// Returns the read-only status of the editor. This is always false if the control cannot move.
	bool readOnly() const { return readOnly_; }
	/// Returns the preferred read-only status of the editor.
	bool readOnlyPreference() const { return readOnlyPreference_; }

signals:
	void moveRequested(double);
	void setpointRequested(double);
	void clicked();
	/// Notifier that the control being edited has changed.
	void controlChanged(AMControl *newControl);
	/// Notifier that the status control has changed.
	void statusControlChanged(AMControl *newControl);
	/// Notifier that the read only status of the editor has changed.
	void readOnlyChanged(bool readOnly);
	/// Notifier that the preferred read-only status of the editor has changed.
	void readOnlyPreferenceChanged(bool readOnly);

public slots:
	/// Sets the control being viewed.
	virtual void setControl(AMControl *newControl);
	/// Sets the status control.
	void setStatusControl(AMControl *newControl);
	/// Sets the preferred read-only status of the editor.
	void setReadOnlyPreference(bool readOnly);
	/// Sets the units text box and prevents them from being set by control updates.
	void setUnits(const QString &newUnits);
	/// Sets the manual override flag for the units text.
	void setUnitsManually(bool manual);
	/// Sets the editor title text.
	void setTitle(const QString &title);
	/// Sets the manual override flag for the editor title text.
	void setTitleManually(bool manual);

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

	/// Sets the units label text.
	void setUnitsText(const QString &newUnits);
	/// Sets the editor title text.
	void setTitleText(const QString &newTitle);
	/// Sets the editor's read-only status.
	void setReadOnly(bool readOnly);

	/// Updates the read-only status of the editor, according to the current control.
	void updateReadOnlyStatus();

	/// Updates the editor when the control connected state changes.
	void onConnectedChanged();
	/// Updates the editor when the control value changes.
	void onValueChanged(double newVal);
	/// Updates the editor when the control units change.
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);

	void onEditStart();
	void onNewSetpoint(double newVal);

	void onStatusValueChanged(double newVal);
	void onControlEnumChanged();

	void onControlMoveStarted();

	/// Handles updating the editor's minimum value in response to a change in the control's minimum value.
	void onControlMinimumValueChanged();
	/// Handles updating the editor's maximum value in response to a change in the control's maximum value.
	void onControlMaximumValueChanged();

protected:
	void mouseReleaseEvent ( QMouseEvent * event );

protected:
	AMControl* control_;
	AMControl* statusTagControl_;
	bool readOnly_;
	bool readOnlyPreference_;
	bool configureOnly_;
	bool connectedOnce_;
	bool newValueOnce_;
	QChar format_;
	int precision_;
	double maxValue_;
	double minValue_;

	QLabel* valueLabel_;
	QLabel* unitsLabel_;
	QHBoxLayout* valueLayout_;
	QHBoxLayout* statusLayout_;
	QVBoxLayout* layout_;
	QLabel* statusLabel_;
	AMExtendedControlEditorStyledInputDialog* dialog_;

	/// Flag that indicates whether or not the units should be updated when the control indicates they have changed.
	bool unitsSetManually_;
	/// Flag that indicates whether or not the editor title should be updated when a new control is set.
	bool titleSetManually_;
};

#endif // AMEXTENDEDCONTROLEDITOR_H
