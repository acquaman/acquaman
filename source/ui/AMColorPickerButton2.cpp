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

