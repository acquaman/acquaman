#include "AMColorPickerButton.h"

#include <QColorDialog>

AMColorPickerButton::AMColorPickerButton(const QColor& initialColor, QWidget *parent) :
	QToolButton(parent)
{
	colorDialog_ = 0;
	setColor(initialColor);
	connect(this, SIGNAL(clicked()), this, SLOT(activateColorBrowser()));
}

AMColorPickerButton::AMColorPickerButton(QWidget *parent) :
	QToolButton(parent)
{
	colorDialog_ = 0;
	setColor(Qt::black);
	connect(this, SIGNAL(clicked()), this, SLOT(activateColorBrowser()));
}

void AMColorPickerButton::activateColorBrowser()
{
	if(!colorDialog_) {
		colorDialog_ = new QColorDialog(currentColor_, this);
		colorDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
		connect(colorDialog_, SIGNAL(accepted()), this, SLOT(onColorDialogAccepted()));
		connect(colorDialog_, SIGNAL(rejected()), this, SLOT(onColorDialogRejected()));
	}

	colorDialog_->show();
}


void AMColorPickerButton::setColor(const QColor &newColor)
{
	currentColor_ = newColor;
	QIcon newIcon;
	QPixmap p = QPixmap(32, 32);
	p.fill(currentColor_);
	newIcon.addPixmap(p);
	setIcon(newIcon);

	emit colorChanged(currentColor_);
}

void AMColorPickerButton::onColorDialogAccepted()
{
	setColor(colorDialog_->currentColor());
	// only necessary to delete the color dialog on Mac os x. (Bug in Qt prevents having two native color dialogs at the same time.) http://bugreports.qt.nokia.com/browse/QTBUG-11188
//#ifdef Q_WS_MAC
//	colorDialog_->deleteLater();
//	colorDialog_ = 0;
//#endif
}

void AMColorPickerButton::onColorDialogRejected() {
	// only necessary to delete the color dialog on Mac os x. (Bug in Qt prevents having two native color dialogs at the same time.) http://bugreports.qt.nokia.com/browse/QTBUG-11188
//#ifdef Q_WS_MAC
//	colorDialog_->deleteLater();
//	colorDialog_ = 0;
//#endif
}

