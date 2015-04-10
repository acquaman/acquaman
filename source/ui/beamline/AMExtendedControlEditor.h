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
	explicit AMExtendedControlEditor(AMControl* control, AMControl* statusTagControl = NULL, bool readOnly = false, bool configureOnly = false, QWidget *parent = 0);

	double setpoint() const;
	AMControl* control() const;

	bool setControlFormat(const QChar& format, int precision);
	void hideBorder();

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

	/// Sets the units text box and prevents them from being set by control updates.
	void setUnits(const QString &newUnits);
	/// Sets the manual override flag for the units text.
	void setUnitsManually(bool manual);

protected slots:
	void setHappy(bool happy = true);
	void setUnhappy() { setHappy(false); }

	/// Sets the units label text to the given newUnits, if the units haven't been overridden.
	void setUnitsText(const QString &newUnits);

	void onValueChanged(double newVal);
	void onUnitsChanged(const QString& units);
	void onMotion(bool moving);

	void onEditStart();
	void onNewSetpoint(double newVal);

	void onStatusValueChanged(double newVal);
	void onControlEnumChanged();

	void onControlMoveStarted();

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
	QHBoxLayout* statusLayout_;
	QLabel* statusLabel_;
	AMExtendedControlEditorStyledInputDialog* dialog_;

	int moveCounter_;

	/// Flag that indicates whether or not the units should be updated when the control indicates they have changed.
	bool unitsSetManually_;
};

#endif // AMEXTENDEDCONTROLEDITOR_H
