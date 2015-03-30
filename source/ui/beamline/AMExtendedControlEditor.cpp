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

#include "AMExtendedControlEditor.h"

#include "beamline/AMControl.h"

#include <QApplication>

#include "AMQEvents.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMenu>
#include <QDebug>

#include "util/AMFontSizes.h"


AMExtendedControlEditor::AMExtendedControlEditor(AMControl* control, AMControl* statusTagControl, bool readOnly, bool configureOnly, QWidget *parent) :
	QGroupBox(parent)
{
	setObjectName("AMControlEdit");

	moveCounter_ = 0;

	control_ = control;
	readOnly_ = readOnly;
	configureOnly_ = configureOnly;
	connectedOnce_ = false;
	newValueOnce_ = false;
	format_ = 'g';
	precision_ = 3;

	statusTagControl_ = statusTagControl;
	if(control_ && !control_->canMove())
		readOnly_ = true;

	// Create objects:
	valueLabel_ = new QLabel("[Not Connected]");
	unitsLabel_ = new QLabel("?");
	if(statusTagControl_){
		QFont statusFont;
		statusFont.setPointSize(10);
		statusLabel_ = new QLabel("[Not Connected]");
		statusLabel_->setFont(statusFont);
		statusLabel_->setMargin(1);
	}
	if(control_){
		if(control_->description() != "")
			setTitle(control_->description());
		else
			setTitle(control_->name());
	}

	// Layout:
	QHBoxLayout* hl = new QHBoxLayout();
	QVBoxLayout* vl = new QVBoxLayout();
	hl->addWidget(valueLabel_, 2);
	hl->addSpacing(2);
	hl->addWidget(unitsLabel_, 0);
	hl->setMargin(2);
	vl->addLayout(hl);
	if(statusTagControl_){
				statusLayout_ = new QHBoxLayout();
				statusLayout_->addWidget(statusLabel_, Qt::AlignCenter);
				statusLayout_->setStretch(0, 2);
				vl->addLayout(statusLayout_);
	}
	vl->setSpacing(1);
	vl->setMargin(2);

	setLayout(vl);

	// Style: TODO: move out of this constructor into app-wide stylesheet
	valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);");
	valueLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		valueLabel_->setAlignment(Qt::AlignCenter);

	setHappy(false);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);


	// Create the editor dialog:
	QStringList enumNames = QStringList();
	dialog_ = new AMExtendedControlEditorStyledInputDialog(enumNames, this);
	dialog_->hide();
	dialog_->setWindowModality(Qt::NonModal);
	if(!configureOnly_)
		connect(dialog_, SIGNAL(doubleValueSelected(double)), control_, SLOT(move(double)));
	else
		connect(dialog_, SIGNAL(doubleValueSelected(double)), this, SLOT(onNewSetpoint(double)));

	// Make connections:
	if(control_){
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		connect(control_, SIGNAL(unitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(setHappy(bool)));
		connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onMotion(bool)));
		connect(control_, SIGNAL(enumChanged()), this, SLOT(onControlEnumChanged()));
		connect(control_, SIGNAL(moveStarted()), this, SLOT(onControlMoveStarted()));
		// If the control is connected already, update our state right now. (We won't get the connected() signal later.)
		if(control_->isConnected()) {
			setHappy(true);
			onValueChanged(control_->value());
			onUnitsChanged(control_->units());
			onMotion(control_->isMoving());
			if(control_->isEnum())
				dialog_->setEnumNames(control_->moveEnumNames());
		}
	}
	if(statusTagControl_) {
		connect(statusTagControl_, SIGNAL(valueChanged(double)), this, SLOT(onStatusValueChanged(double)));
		if(statusTagControl_->isConnected())
			onStatusValueChanged(statusTagControl_->value());

	}

	connect(this, SIGNAL(clicked()), this, SLOT(onEditStart()));

}

double AMExtendedControlEditor::setpoint() const{
	return dialog_->setpoint();
}

AMControl* AMExtendedControlEditor::control() const{
	return control_;
}

bool AMExtendedControlEditor::setControlFormat(const QChar& format, int precision){
	if(format == 'g' || format == 'G' || format == 'e' || format == 'E' || format == 'f'){
		format_ = format;
		precision_ = precision;
		dialog_->setDoubleDecimals(precision_);
		if(control_->isConnected())
			onValueChanged(control_->value());
		return true;
	}
	return false;
}

void AMExtendedControlEditor::setReadOnly(bool readOnly){
	readOnly_ = readOnly;
	if(!control_->canMove())
		readOnly_ = true;
}

void AMExtendedControlEditor::setNoUnitsBox(bool noUnitsBox){
	if(noUnitsBox && !unitsLabel_->isHidden())
		unitsLabel_->hide();
	else if(!noUnitsBox && unitsLabel_->isHidden())
		unitsLabel_->show();
}

void AMExtendedControlEditor::overrideTitle(const QString &title){
	setTitle(title);
}

void AMExtendedControlEditor::setSetpoint(double newSetpoint){
	dialog_->setDoubleValue(newSetpoint);
	onNewSetpoint(newSetpoint);
}

void AMExtendedControlEditor::onValueChanged(double newVal) {
	if(configureOnly_ && connectedOnce_)
		return;
	if(control_->isEnum()){
		valueLabel_->setText(control_->enumNameAt(newVal));
		unitsLabel_->setText("");
	}
	else
		valueLabel_->setText(QString("%1").arg(newVal, 0, format_.toAscii(), precision_));
}

void AMExtendedControlEditor::onUnitsChanged(const QString& units) {
	if(configureOnly_ && connectedOnce_)
		return;
	if(control_->isEnum())
		unitsLabel_->setText("");
	else
		unitsLabel_->setText(units);
}


void AMExtendedControlEditor::setHappy(bool happy) {
	if(happy){
		unitsLabel_->setStyleSheet("border: 1px outset #00df00; background: #d4ffdf; padding: 1px; width: 100%; color: #00df00;");
		readOnly_ = !control_->canMove();
		onUnitsChanged(control_->units());
		onValueChanged(control_->value());
		if(!connectedOnce_){
			dialog_->setDoubleMaximum(control_->maximumValue());
			dialog_->setDoubleMinimum(control_->minimumValue());
			dialog_->setDoubleValue(control_->value());
			connectedOnce_ = true;
		}
	}
	else
		unitsLabel_->setStyleSheet("border: 1px outset #f20000; background: #ffdfdf;	padding: 1px; color: #f20000;");
}

void AMExtendedControlEditor::onMotion(bool moving) {
	if(moving)
		unitsLabel_->setStyleSheet("border: 1px outset blue; background: #ffdfdf;	padding: 1px; color: blue;");
	else
		setHappy(control_->isConnected());
}

void AMExtendedControlEditor::onEditStart() {

	if(readOnly_ || !control_->canMove()) {
		QApplication::beep();
		return;
	}

	dialog_->setDoubleMaximum(control_->maximumValue());
	dialog_->setDoubleMinimum(control_->minimumValue());

	if(configureOnly_ && control_->isEnum() && control_->moveEnumNames().contains(valueLabel_->text()))
		dialog_->setDoubleValue(control_->moveEnumNames().indexOf(valueLabel_->text()));
	else if (control_->isEnum() && control_->moveEnumNames().contains(valueLabel_->text()))
		dialog_->setDoubleValue(control_->moveEnumNames().indexOf(valueLabel_->text()));
	else if(configureOnly_ && ! control_->isEnum()){
		bool conversionOk = false;
		double valueForText = valueLabel_->text().toDouble(&conversionOk);
		if(conversionOk)
			dialog_->setDoubleValue(valueForText);
	}
	else
		dialog_->setDoubleValue(control_->value());
	dialog_->setDoubleDecimals(precision_);
	dialog_->setLabelText(control_->objectName());
	dialog_->setSuffix(control_->units());
	dialog_->show();
	dialog_->move( mapToGlobal(QPoint(width()/2,height()/2)) - QPoint(dialog_->width()/2, dialog_->height()/2) );

}

void AMExtendedControlEditor::onNewSetpoint(double newVal){
	if(!configureOnly_)
		return;
	if(control_->isEnum()){
		if( fabs(control_->value()-newVal) < control_->tolerance() )
			valueLabel_->setText(QString("%1").arg(control_->enumNameAt(newVal)));
		else
			valueLabel_->setText(QString("%1 (from %2)").arg(control_->enumNameAt(newVal)).arg(control_->enumNameAt(control_->value())) );
		unitsLabel_->setText("");
	}
	else{
		if( fabs(control_->value()-newVal) < control_->tolerance() )
			valueLabel_->setText(QString("%1").arg(newVal));
		else
			valueLabel_->setText(QString("%1 (from %2)").arg(newVal).arg(control_->value()));
	}
	emit setpointRequested(newVal);
}

void AMExtendedControlEditor::onStatusValueChanged(double newVal){
	if(statusTagControl_->isEnum())
		statusLabel_->setText(statusTagControl_->enumNameAt(newVal));
	else
		statusLabel_->setText(QString("%1").arg(newVal));
}

QSize AMExtendedControlEditor::sizeHint() const{
	QSize newHint = QGroupBox::sizeHint();
	newHint.setHeight(newHint.height()+6);
	return newHint;
}

void AMExtendedControlEditor::mouseReleaseEvent ( QMouseEvent * event ) {
	if(event->button() == Qt::LeftButton) {
		event->accept();
		emit clicked();
	}
	else
		event->ignore();

}

 AMExtendedControlEditorStyledInputDialog::~AMExtendedControlEditorStyledInputDialog(){}
AMExtendedControlEditorStyledInputDialog::AMExtendedControlEditorStyledInputDialog( QStringList enumNames, QWidget * parent, Qt::WindowFlags flags ) : QDialog(parent, flags) {
	setObjectName("styledDialog");
	setStyleSheet("#styledDialog { background-color: rgb(31,62,125); border: 2px outset white; border-radius: 10px; }  QLabel { color: white; font: bold " AM_FONT_REGULAR_ " \"Helvetica\"; } QPushButton { color: white; border: 1px outset rgb(158,158,158); border-radius: 5px; min-height: 24px; padding: 3px; width: 80px; margin: 3px;} #okButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(191, 218, 178, 255), stop:0.34 rgba(135, 206, 96, 255), stop:1 rgba(65, 157, 0, 255));} #cancelButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(232, 209, 209, 255), stop:0.34 rgba(229, 112, 119, 255), stop:1 rgba(197, 20, 32, 255)); } QDoubleSpinBox { padding: 3px; color: black; font: bold " AM_FONT_REGULAR_ " \"Helvetica\"; border: 1px outset rgb(158,158,158); selection-background-color: rgb(205, 220, 243); selection-color: black;}");// TODO: continue here...

	label_ = new QLabel("New value:");
	label_->setAlignment(Qt::AlignCenter);
	enumNames_ = enumNames;

	isEnum_ = false; //IS THIS THE MISSING PIECE?
	if(enumNames_.count() > 0)
		isEnum_ = true;
	spinBox_ = new QDoubleSpinBox();
	spinBox_->setObjectName("valueEntry");
	spinBox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
	comboBox_ = new QComboBox();
	comboBox_->setObjectName("valueEntry");
	comboBox_->addItems(enumNames_);
	okButton_ = new QPushButton("Ok");
	okButton_->setObjectName("okButton");
	cancelButton_ = new QPushButton("Cancel");
	cancelButton_->setObjectName("cancelButton");

	vl_ = new QVBoxLayout();
	vl_->addWidget(label_);
	if(!isEnum_){
		vl_->addWidget(spinBox_);
		comboBox_->hide();
	}
	else{
		vl_->addWidget(comboBox_);
		spinBox_->hide();
	}

	hl_ = new QHBoxLayout();
	hl_->addWidget(cancelButton_);
	hl_->addWidget(okButton_);
	vl_->addLayout(hl_);

	setLayout(vl_);

	connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(reject()));

	okButton_->setDefault(true);
}

double AMExtendedControlEditorStyledInputDialog::setpoint() const{
	if(!isEnum_)
		return spinBox_->value();
	else
		return comboBox_->currentIndex();
}

void AMExtendedControlEditorStyledInputDialog::setDoubleValue(double d) {
	if(!isEnum_)
		spinBox_->setValue(d);
	else
		comboBox_->setCurrentIndex((int)d);
}

void AMExtendedControlEditorStyledInputDialog::setDoubleMaximum(double d) {
	if(!isEnum_)
		spinBox_->setMaximum(d);
}

void AMExtendedControlEditorStyledInputDialog::setDoubleMinimum(double d) {
	if(!isEnum_)
		spinBox_->setMinimum(d);
}

void AMExtendedControlEditorStyledInputDialog::setDoubleDecimals(int d) {
	if(!isEnum_)
		spinBox_->setDecimals(d);
}

void AMExtendedControlEditorStyledInputDialog::setLabelText(const QString& s) {
	label_->setText(s);
}

void AMExtendedControlEditorStyledInputDialog::setEnumNames(const QStringList &sl){

	bool oldIsEnum = isEnum_;
	if(sl.count() > 0)
		isEnum_ = true;
	else
		isEnum_ = false;
	enumNames_ = sl;
	comboBox_->clear();
	comboBox_->addItems(enumNames_);
	if(oldIsEnum != isEnum_){
		if(!isEnum_){
			vl_->removeWidget(comboBox_);
			vl_->insertWidget(1, spinBox_);
			comboBox_->hide();
			spinBox_->show();
		}
		else{
			vl_->removeWidget(spinBox_);
			vl_->insertWidget(1, comboBox_);
			spinBox_->hide();
			comboBox_->show();
		}
	}
}

void AMExtendedControlEditorStyledInputDialog::setSuffix(const QString& s) {
	spinBox_->setSuffix(QString(" ") + s);
}

void AMExtendedControlEditorStyledInputDialog::onAccepted() {
	double eVal;
	if(!isEnum_)
		eVal = spinBox_->value();
	else
		eVal = comboBox_->currentIndex();
	emit doubleValueSelected(eVal);
}

void AMExtendedControlEditorStyledInputDialog::resizeEvent(QResizeEvent *  event )
{
	QDialog::resizeEvent(event);

	// Create a rounded-rectangle mask to shape this window:
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), 14, 14);
	QPolygonF polygonf = path.toFillPolygon(QTransform());
	QRegion maskedRegion( polygonf.toPolygon() );
	setMask(maskedRegion);

}

void AMExtendedControlEditorStyledInputDialog::showEvent ( QShowEvent * event ) {
	QDialog::showEvent(event);
	if(!isEnum_)
		spinBox_->setFocus();
	else
		comboBox_->setFocus();
}

void AMExtendedControlEditor::onControlEnumChanged()
{
	dialog_->setEnumNames(control_->moveEnumNames());
}

void AMExtendedControlEditor::onControlMoveStarted(){
	/*
	moveCounter_++;
	moveCounter_ = moveCounter_%4;
	switch(moveCounter_){
	case 0:
		valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);");
		break;
	case 1:
		valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #0198E1, stop: 0.25 #0198E1, stop: 0.26 #FFFFFF, stop: 1.0 #FFFFFF);");
		break;
	case 2:
		valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #0198E1, stop: 0.5 #0198E1, stop: 0.51 #FFFFFF, stop: 1.0 #FFFFFF);");
		break;
	case 3:
		valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 #0198E1, stop: 0.75 #0198E1, stop: 0.76 #FFFFFF, stop: 1.0 #FFFFFF);");
		break;
	}
	if(moveCounter_ != 0)
		QTimer::singleShot(500, this, SLOT(onControlMoveStarted()));
	*/
}
