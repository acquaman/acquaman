#include "AMColorPickerButton2.h"

#include <QColorDialog>

AMColorPickerButton2::AMColorPickerButton2(const QColor& initialColor, QWidget *parent) :
	QToolButton(parent)
{
	colorDialog_ = 0;
	setColor(initialColor);
	connect(this, SIGNAL(clicked()), this, SLOT(activateColorBrowser()));
}

void AMColorPickerButton2::activateColorBrowser()
{
	if(!colorDialog_) {
		colorDialog_ = new QColorDialog(currentColor_, this);
		colorDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
		connect(colorDialog_, SIGNAL(accepted()), this, SLOT(onColorDialogAccepted()));
	}

	colorDialog_->show();
}


void AMColorPickerButton2::setColor(const QColor &newColor)
{
	currentColor_ = newColor;
	QIcon newIcon;
	QPixmap p = QPixmap(32, 32);
	p.fill(currentColor_);
	newIcon.addPixmap(p);
	setIcon(newIcon);

	emit colorChanged(currentColor_);
}

void AMColorPickerButton2::onColorDialogAccepted()
{
	setColor(colorDialog_->currentColor());
}

