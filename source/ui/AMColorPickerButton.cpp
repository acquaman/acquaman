#include "AMColorPickerButton.h"

#include <QColorDialog>

AMColorPickerButton::AMColorPickerButton(const QColor& initialColor, QWidget *parent) :
	QToolButton(parent)
{
	colorDialog_ = 0;
	setColor(initialColor);
	connect(this, SIGNAL(clicked()), this, SLOT(activateColorBrowser()));
}

void AMColorPickerButton::activateColorBrowser()
{
	if(!colorDialog_) {
		colorDialog_ = new QColorDialog(currentColor_, this);
		colorDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
		connect(colorDialog_, SIGNAL(accepted()), this, SLOT(onColorDialogAccepted()));
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
}

