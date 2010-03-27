#include "NumericControl.h"
#include <QApplication>
#include <QMouseEvent>

#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

NumericControl::NumericControl(AMControl* control, QWidget *parent) :
	QFrame(parent)
{
	setObjectName("NumericControl");

	control_ = control;

	// Create objects:
	valueLabel_ = new QLabel("[unconnected]");
	unitsLabel_ = new QLabel("?");


	// Layout:
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(valueLabel_, 2);
	hl->addSpacing(2);
	hl->addWidget(unitsLabel_, 0);
	hl->setMargin(2);

	setLayout(hl);

	// Style: TODO: move out of this constructor into app-wide stylesheet
	valueLabel_->setStyleSheet("color: rgb(0, 0, 0); background-color: rgb(255, 255, 255);");
	valueLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setFrameStyle(QFrame::StyledPanel);
	setStyleSheet("QFrame#NumericControl { background: white; } ");
	setHappy(false);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


	// Make connections:
	if(control_) {
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		connect(control_, SIGNAL(unitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(setHappy(bool)));
		connect(control_, SIGNAL(moving(bool)), this, SLOT(onMotion(bool)));
	}
	connect(this, SIGNAL(clicked()), this, SLOT(onEditStart()));
	
	// Create the editor dialog:
	dialog_ = new StyledInputDialog(this);
	// dialog_->setInputMode(QInputDialog::DoubleInput);
	dialog_->hide();
	dialog_->setWindowModality(Qt::NonModal);
	connect(dialog_, SIGNAL(doubleValueSelected(double)), control_, SLOT(move(double)));

}

void NumericControl::onValueChanged(double newVal) {
	valueLabel_->setText(QString("%1").arg(newVal));
}
void NumericControl::onUnitsChanged(const QString& units) {
	unitsLabel_->setText(units);
}


void NumericControl::setHappy(bool happy) {
	if(happy)
		unitsLabel_->setStyleSheet("border: 1px outset #00df00; background: #d4ffdf; padding: 1px; width: 100%; color: #00df00;");
	else
		unitsLabel_->setStyleSheet("border: 1px outset #f20000; background: #ffdfdf;	padding: 1px; color: #f20000;");
}

void NumericControl::onMotion(bool moving) {
	if(moving)
		unitsLabel_->setStyleSheet("border: 1px outset blue; background: #ffdfdf;	padding: 1px; color: blue;");
	else
		setHappy(control_->isConnected());
}

void NumericControl::onEditStart() {

	if(!control_->canMove()) {
		QApplication::beep();
		return;
	}
	
	bool ok;

	dialog_->setDoubleValue(control_->value());
	dialog_->setDoubleMaximum(control_->maximumValue());
	dialog_->setDoubleMinimum(control_->minimumValue());
	dialog_->setDoubleDecimals(3);	// todo: display precision?
	dialog_->setLabelText(control_->objectName());
	dialog_->setSuffix(control_->units());
	dialog_->show();
	dialog_->move( mapToGlobal(QPoint(width()/2,height()/2)) - QPoint(dialog_->width()/2, dialog_->height()/2) );

}

void NumericControl::mouseReleaseEvent ( QMouseEvent * event ) {
	if(event->button() == Qt::LeftButton) {
		event->accept();
		emit clicked();
	}
	else
		event->ignore();

}

void StyledInputDialog::resizeEvent(QResizeEvent *  event )
{
	QDialog::resizeEvent(event);


	int side = qMin(width(), height());
	// Create a rounded-rectangle mask to shape this window:
	QPainterPath path;
	path.addRoundedRect(0, 0, width(), height(), 14, 14);
	QPolygonF polygonf = path.toFillPolygon(QTransform());
	QRegion maskedRegion( polygonf.toPolygon() );
	setMask(maskedRegion);

}

StyledInputDialog::StyledInputDialog( QWidget * parent, Qt::WindowFlags flags ) : QDialog(parent, flags) {
	setObjectName("styledDialog");
	setStyleSheet("#styledDialog { background-color: rgb(31,62,125); border: 2px outset white; border-radius: 10px; }  QLabel { color: white; font: bold 12pt \"Helvetica\"; } QPushButton { color: white; /* font: bold 12pt  \"Helvetica\"*/; border: 1px outset rgb(158,158,158); border-radius: 5px; min-height: 24px; padding: 3px; width: 80px; margin: 3px;} #okButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(191, 218, 178, 255), stop:0.34 rgba(135, 206, 96, 255), stop:1 rgba(65, 157, 0, 255));} #cancelButton { background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(232, 209, 209, 255), stop:0.34 rgba(229, 112, 119, 255), stop:1 rgba(197, 20, 32, 255)); } QDoubleSpinBox { padding: 3px; color: black; font: bold 12pt \"Helvetica\"; border: 1px outset rgb(158,158,158); selection-background-color: rgb(205, 220, 243); selection-color: black;}");// TODO: continue here...
	// setStyleSheet("#styledDialog {  border: 1px outset black; }  QLabel { color: black; font: bold 12pt \"Helvetica\"; } QPushButton { min-height: 24px; padding: 3px; width: 80px; margin: 3px;} #okButton {border: 1px outset #00df00; background: #d4ffdf; padding: 1px; width: 100%; color: #00df00; } #cancelButton { border: 1px outset #f20000; background: #ffdfdf;	padding: 1px; color: #f20000; } QDoubleSpinBox { padding: 3px; color: black; font: bold 12pt \"Helvetica\"; border: 1px outset rgb(158,158,158); selection-background-color: rgb(205, 220, 243); selection-color: black;}");// TODO: continue here...

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

void StyledInputDialog::onAccepted() {
	emit doubleValueSelected(spinBox_->value());
}

void StyledInputDialog::setDoubleValue(double d) { spinBox_->setValue(d); }
void StyledInputDialog::setDoubleMaximum(double d) { spinBox_->setMaximum(d); }
void StyledInputDialog::setDoubleMinimum(double d) { spinBox_->setMinimum(d); }
void StyledInputDialog::setDoubleDecimals(double d) { spinBox_->setDecimals(d); }
void StyledInputDialog::setLabelText(const QString& s) { label_->setText(s); }
void StyledInputDialog::setSuffix(const QString& s) { spinBox_->setSuffix(QString(" ") + s); }

void StyledInputDialog::showEvent ( QShowEvent * event ) { QDialog::showEvent(event); spinBox_->setFocus(); }
