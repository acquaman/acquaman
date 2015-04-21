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


#include "AMControlEditor.h"

#include "beamline/AMControl.h"

#include <QApplication>

#include "AMQEvents.h"

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>
#include <QLabel>
#include <QFont>
#include <QPixmap>
#include <QMovie>
#include <QMenu>

#include "util/AMFontSizes.h"

QMovie* AMControlEditor::movingIcon_ = 0;
QPixmap* AMControlEditor::invalidIcon_ = 0;
QPixmap* AMControlEditor::minorIcon_ = 0;
QPixmap* AMControlEditor::majorIcon_ = 0;
QPixmap* AMControlEditor::lockedIcon_ = 0;

AMControlEditor::AMControlEditor(AMControl* control, AMControl* secondaryControl, QWidget *parent) :
	QFrame(parent)
{
	setObjectName("AMControlEditor");

	control_ = 0;
	secondaryControl_ = 0;
	readOnly_ = false;

	// create static caches, if not already here:
	if(!movingIcon_) {
		movingIcon_ = new QMovie(":/16x16/loading2_transparent.gif");
		movingIcon_->start();
	}
	if(!invalidIcon_)
		invalidIcon_ = new QPixmap(":/16x16/network-error.png");
	if(!minorIcon_)
		minorIcon_ = new QPixmap(":/16x16/dialog-warning.png");
	if(!majorIcon_)
		majorIcon_ = new QPixmap(":/22x22/redCrash.png");
	if(!lockedIcon_)
		lockedIcon_ = new QPixmap(":/16x16/locked.png");

	// Create objects. Thanks to implicit sharing of QPixmap, all instances of this widget will all use the same QPixmap storage, created in the static icons.
	valueLabel_ = new QLabel();
	enumButton_ = new QToolButton();
	enumMenu_ = new QMenu(this);
	enumButton_->setMenu(enumMenu_);
	enumButton_->setPopupMode(QToolButton::InstantPopup);
	connect(enumMenu_, SIGNAL(triggered(QAction*)), this, SLOT(onEnumValueChosen(QAction*)));

	movingLabel_ = new QLabel();
	movingLabel_->setMovie(movingIcon_);
	movingLabel_->setToolTip("Moving...");

	invalidLabel_ = new QLabel();
	invalidLabel_->setPixmap(*invalidIcon_);

	minorLabel_ = new QLabel();
	minorLabel_->setPixmap(*minorIcon_);
	minorLabel_->setToolTip("Alarm: Minor");

	majorLabel_ = new QLabel();
	majorLabel_->setPixmap(*majorIcon_);
	majorLabel_->setToolTip("Alarm: Major");

	lockedLabel_ = new QLabel();
	lockedLabel_->setPixmap(*lockedIcon_);
	lockedLabel_->setToolTip("Cannot Move");

	statusFrame_ = new QFrame();
	statusFrame_->setMinimumHeight(22);
	statusFrame_->setObjectName("StatusFrame");

	QHBoxLayout* shl = new QHBoxLayout();
	shl->addWidget(movingLabel_);
	shl->addWidget(lockedLabel_);
	shl->addWidget(invalidLabel_);
	shl->addWidget(minorLabel_);
	shl->addWidget(majorLabel_);
	shl->setContentsMargins(0,0,0,0);
	shl->setSpacing(1);
	statusFrame_->setLayout(shl);
	movingLabel_->hide();

	// Layout:
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(valueLabel_, 2);
	hl->addWidget(enumButton_, 2);
	hl->addWidget(statusFrame_, 0);
	hl->setSpacing(0);
	hl->setContentsMargins(2,2,2,2);

	setLayout(hl);
	enumButton_->hide();

	valueLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	enumButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	setFrameStyle(QFrame::StyledPanel);
	setStyleSheet("#AMControlEditor { background: white; } ");
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	// Create the editor dialog:
	dialog_ = new AMControlEditorStyledInputDialog(this);
	dialog_->hide();
	dialog_->setWindowModality(Qt::NonModal);
	connect(dialog_, SIGNAL(doubleValueSelected(double)), this, SLOT(onNewSetpointChosen(double)));

	// Apply current control settings.
	setControl(control);
	setSecondaryControl(secondaryControl);

	connect(this, SIGNAL(clicked()), this, SLOT(onEditStart()));
}

void AMControlEditor::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );

			valueLabel_->clear();
			enumButton_->hide();
			movingLabel_->hide();

		}

		control_ = newControl;

		if (control_) {
			connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
			connect(control_, SIGNAL(connected(bool)), this, SLOT(reviewControlState()));
			connect(control_, SIGNAL(unitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));
			connect(control_, SIGNAL(alarmChanged(int,int)), this, SLOT(reviewControlState()));
			connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMotion(bool)));
			connect(control_, SIGNAL(enumChanged()), this, SLOT(onEnumChanged()));

			// If the control is connected already, update our state right now.
			if(control_->isConnected()) {
				onValueChanged(control_->value());
				onUnitsChanged(control_->units());
				onMotion(control_->isMoving());
				onEnumChanged();
			}
		}

		reviewControlState();

		emit controlChanged(control_);
	}
}

void AMControlEditor::setSecondaryControl(AMControl *newControl)
{
	if (secondaryControl_ != newControl) {

		if (secondaryControl_) {
			disconnect( secondaryControl_, 0, this, 0 );

			valueLabel_->clear();
		}

		secondaryControl_ = newControl;

		if (secondaryControl_) {
			connect( secondaryControl_, SIGNAL(connected(bool)), this, SLOT(onSecondaryConnectedChanged()) );
			connect(secondaryControl_, SIGNAL(valueChanged(double)), this, SLOT(onSecondaryValueChanged(double)));
			connect(secondaryControl_, SIGNAL(unitsChanged(QString)), this, SLOT(onSecondaryUnitsChanged(QString)));

			onSecondaryConnectedChanged();

			// if the secondary control is null but the primary control is not, can update value and units labels.
		} else if (control_ && control_->isConnected()) {
			onValueChanged(control_->value());
			onUnitsChanged(control_->units());
		}
	}
}

void AMControlEditor::setReadOnly(bool readOnly) {
	readOnly_ = readOnly;

	// disable the popup menu for the enum button
	enumButton_->setMenu(readOnly_ ? 0 : enumMenu_);

	// update whether the lock label is shown. It's shown if we cannot move the control, or if this widget is configured read-only.
	reviewControlState();
}

void AMControlEditor::onValueChanged(double newVal) {
	if(control_) {
		if (control_->isEnum())
			enumButton_->setText(control_->enumNameAt(newVal));
		else
			valueLabel_->setText(QString("%1 %2").arg(newVal).arg(control_->units()));
	}
}

void AMControlEditor::onUnitsChanged(const QString &units) {
	if(control_ && control_->isConnected())
	{
		if(secondaryControl_ && secondaryControl_->isConnected())
			valueLabel_->setText(QString("%1 %2 (%3 %4)").arg(control_->value()).arg(units).arg(secondaryControl_->value()).arg(secondaryControl_->units()));
		else
			valueLabel_->setText(QString("%1 %2").arg(control_->value()).arg(units));
	}
}

void AMControlEditor::reviewControlState() {
	invalidLabel_->hide();
	minorLabel_->hide();
	majorLabel_->hide();
	lockedLabel_->setVisible(readOnly_);

	if(!control_ || !control_->isConnected()) {
		invalidLabel_->show();
		invalidLabel_->setToolTip("Not Connected");
		setStyleSheet("#AMControlEditor {border: 1px outset #f20000; background: #ffffff;}");
		return;
	}

	lockedLabel_->setVisible(!control_->canMove() || readOnly_);

	switch(control_->alarmSeverity()) {
	case AMControl::InvalidAlarm:
		invalidLabel_->show();
		invalidLabel_->setToolTip("Alarm: Invalid");
		setStyleSheet("#AMControlEditor {border: 1px outset #f20000; background: #ffffff;}");
		break;
	case AMControl::MinorAlarm:
		minorLabel_->show();
		setStyleSheet("#AMControlEditor {border: 1px outset rgb(242,242,0); background: rgb(255,255,240);}");
		break;
	case AMControl::MajorAlarm:
		majorLabel_->show();
		setStyleSheet("#AMControlEditor {border: 1px outset #f20000; background: #ffdfdf;}");
		break;
	default: // all good. Normal look.
		setStyleSheet("#AMControlEditor { background: white; } ");
		break;
	}
}

void AMControlEditor::onEnumChanged() {
	if(control_ && control_->isEnum()) {
		enumButton_->show();
		valueLabel_->hide();

		// With new enum values, we might have to update the text value on the button.
		onValueChanged(control_->value());

		enumMenu_->clear();
		int i=0;
		foreach(QString enumString, control_->moveEnumNames()) {
			QAction* action = enumMenu_->addAction(enumString);
			action->setData(i++);	// remember the index inside this action.
		}
	}
	else {
		enumButton_->hide();
		valueLabel_->show();
	}
}

void AMControlEditor::onEnumValueChosen(QAction *action) {
	int value = action->data().toInt();
	onNewSetpointChosen(double(value));
}

void AMControlEditor::onMotion(bool moving) {
	if(moving)
		movingLabel_->show();
	else
		movingLabel_->hide();
}

void AMControlEditor::onEditStart() {

	if(!control_ || !control_->canMove() || readOnly_) {
		QApplication::beep();
		return;
	}

	if(control_->isEnum())
		return;	// don't show the editor in this situation, users use the tool button menu directly

	dialog_->setDoubleMaximum(control_->maximumValue());
	dialog_->setDoubleMinimum(control_->minimumValue());
	dialog_->setDoubleDecimals(3);
	dialog_->setDoubleValue(control_->value());
	dialog_->setLabelText(control_->description().isEmpty() ? control_->name() : control_->description());
	dialog_->setSuffix(control_->units());
	dialog_->show();
	dialog_->move( mapToGlobal(QPoint(width()/2,height()/2)) - QPoint(dialog_->width()/2, dialog_->height()/2) );

}

void AMControlEditor::mouseReleaseEvent ( QMouseEvent * event ) {
	if(event->button() == Qt::LeftButton) {
		event->accept();
		emit clicked();
	}
	else
		event->ignore();

}

void AMControlEditorStyledInputDialog::resizeEvent(QResizeEvent *  event )
{
	QDialog::resizeEvent(event);

	// Create a rounded-rectangle mask to shape this window:
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), 14, 14);
	QPolygonF polygonf = path.toFillPolygon(QTransform());
	QRegion maskedRegion( polygonf.toPolygon() );
	setMask(maskedRegion);

}

 AMControlEditorStyledInputDialog::~AMControlEditorStyledInputDialog(){}
AMControlEditorStyledInputDialog::AMControlEditorStyledInputDialog( QWidget * parent, Qt::WindowFlags flags ) : QDialog(parent, flags) {
	setObjectName("styledDialog");
	setStyleSheet("#styledDialog { background-color: rgb(31,62,125); border: 2px outset white; border-radius: 10px; }  QLabel { color: white; font: bold " AM_FONT_REGULAR_ " \"Helvetica\"; } QPushButton { color: white; border: 1px outset rgb(158,158,158); border-radius: 5px; min-height: 24px; padding: 3px; width: 80px; margin: 3px;} #okButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(191, 218, 178, 255), stop:0.34 rgba(135, 206, 96, 255), stop:1 rgba(65, 157, 0, 255));} #cancelButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(232, 209, 209, 255), stop:0.34 rgba(229, 112, 119, 255), stop:1 rgba(197, 20, 32, 255)); } QDoubleSpinBox { padding: 3px; color: black; font: bold " AM_FONT_REGULAR_ " \"Helvetica\"; border: 1px outset rgb(158,158,158); selection-background-color: rgb(205, 220, 243); selection-color: black;}");

	label_ = new QLabel("New value:");
	label_->setAlignment(Qt::AlignCenter);
	spinBox_ = new QDoubleSpinBox();
	spinBox_->setObjectName("valueEntry");
	spinBox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	okButton_ = new QPushButton("Ok");
	okButton_->setObjectName("okButton");
	cancelButton_ = new QPushButton("Cancel");
	cancelButton_->setObjectName("cancelButton");

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(label_);
	vl->addWidget(spinBox_);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(cancelButton_);
	hl->addWidget(okButton_);
	vl->addLayout(hl);

	setLayout(vl);

	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(reject()));

	okButton_->setDefault(true);
}

void AMControlEditorStyledInputDialog::onAccepted() {
	emit doubleValueSelected(spinBox_->value());
}

void AMControlEditorStyledInputDialog::setDoubleValue(double d) { spinBox_->setValue(d); }
void AMControlEditorStyledInputDialog::setDoubleMaximum(double d) { spinBox_->setMaximum(d); }
void AMControlEditorStyledInputDialog::setDoubleMinimum(double d) { spinBox_->setMinimum(d); }
void AMControlEditorStyledInputDialog::setDoubleDecimals(int d) { spinBox_->setDecimals(d); }
void AMControlEditorStyledInputDialog::setLabelText(const QString& s) { label_->setText(s); }
void AMControlEditorStyledInputDialog::setSuffix(const QString& s) { spinBox_->setSuffix(QString(" ") + s); }

void AMControlEditorStyledInputDialog::showEvent ( QShowEvent * event ) { QDialog::showEvent(event); spinBox_->setFocus(); spinBox_->selectAll(); }


void AMControlEditor::onNewSetpointChosen(double value)
{
	if(control_)
		control_->move(value);
}

void AMControlEditor::onDisplayPrecisionChanged(int displayPrecision)
{
	dialog_->setDoubleDecimals(displayPrecision);
}

void AMControlEditor::onSecondaryConnectedChanged()
{
	if (secondaryControl_ && secondaryControl_->isConnected()) {
		onSecondaryValueChanged(secondaryControl_->value());
		onSecondaryUnitsChanged(secondaryControl_->units());
	}
}

void AMControlEditor::onSecondaryValueChanged(double newValue)
{
	if (control_ && secondaryControl_)
		valueLabel_->setText(QString("%1 %2 (%3 %4)").arg(control_->value()).arg(control_->units()).arg(newValue).arg(secondaryControl_->units()));
}

void AMControlEditor::onSecondaryUnitsChanged(const QString &units)
{
	if(control_ && control_->isConnected())
	{
		if(secondaryControl_ && secondaryControl_->isConnected())
			valueLabel_->setText(QString("%1 %2 (%3 %4)").arg(control_->value()).arg(units).arg(secondaryControl_->value()).arg(secondaryControl_->units()));
		else
			valueLabel_->setText(QString("%1 %2").arg(control_->value()).arg(units));
	}
}

