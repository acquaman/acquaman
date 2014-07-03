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


#include "ui/CLS/CLSStopLightButton.h"
#include <QPainter>
#include <QRadialGradient>
#include <QStyleOption>
#include <QFontMetrics>

 CLSStopLightButton::~CLSStopLightButton(){}
CLSStopLightButton::CLSStopLightButton(CLSBiStateControl *control, QWidget *parent)
	: QPushButton(parent)
{
	control_ = control;
	connect(control_, SIGNAL(stateChanged(int)), this, SLOT(update()));
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));

	setFlat(true);

	// Determining the size of the button.
	QFontMetrics metric(this->font());
	int width = metric.width(control_->name());

	if (width < 40)
		width = 40;

	setMinimumSize(int(1.4*width), int(1.4*width));
}

void CLSStopLightButton::mouseReleaseEvent(QMouseEvent *e)
{
	if (isFlat())
		return;

	QPushButton::mouseReleaseEvent(e);
}

void CLSStopLightButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	if (!isFlat()){
		// Main button box.  Uses the standand push button style for the basic aspects of the box.
		QStyleOptionButton option;
		option.initFrom(this);
		option.state |= isDown() ? QStyle::State_Sunken : QStyle::State_Raised;
		style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
	}

	qreal w = width();
	qreal h = height();

	// Draw a black box to act like a case.
	painter.setBrush(QBrush(Qt::black));
	painter.translate(0.5*w-0.4*h, 0.1*h);
	painter.drawRect(QRectF(0, 0, 0.8*h, 0.8*h));

	// Paint the center light.
	QRadialGradient lightGradient(0.45*h, 0.3*h, 0.55*h);
	switch(control_->state()){
	case 1:
		lightGradient.setColorAt(0, QColor(0, 200, 0));
		lightGradient.setColorAt(0.5, QColor(34, 139, 34));
		break;
	case 2:
		lightGradient.setColorAt(0, QColor(255, 255, 0));
		lightGradient.setColorAt(0.5, QColor(200, 200, 0));
		break;
	case 0:
		lightGradient.setColorAt(0, QColor(255, 0, 0));
		lightGradient.setColorAt(0.5, QColor(200, 0, 0));
		break;
	}
	QBrush lightBrush(lightGradient);
	painter.setBrush(lightBrush);
	painter.drawEllipse(QRectF(0.09*h, 0.09*h, 0.64*h, 0.64*h));

	// Paint on the title.
	QFontMetrics metric(this->font());
	if (control_->state() == 2)
		painter.setPen(Qt::darkBlue);
	else
		painter.setPen(Qt::white);

	QString title(control_->name());
	painter.drawText(QRectF((0.8*h-metric.width(title))/2, 0.25*h, metric.width(title), metric.height()), title);
}
