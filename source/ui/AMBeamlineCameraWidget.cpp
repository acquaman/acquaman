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


#include "AMBeamlineCameraWidget.h"

#include <QCheckBox>
#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QSlider>

#include <QMessageBox>
#include <QStringBuilder>

#include "ui/AMCrosshairOverlayVideoWidget.h"
#include "ui/dataman/AMColorPickerButton.h"

 AMBeamlineCameraWidget::~AMBeamlineCameraWidget(){}
AMBeamlineCameraWidget::AMBeamlineCameraWidget(QWidget *parent, bool useOpenGlViewport) :
	QWidget(parent)
{
	crosshairLocked_ = false;

	setWindowTitle("Video");

	// GUI setup:
	//////////////////////////
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setSpacing(0);
	vl->setContentsMargins(0,0,0,0);

	QFrame* crosshairFrame = new QFrame();
	QHBoxLayout* chl = new QHBoxLayout();
	chl->setContentsMargins(12,4,12,4);
	chl->addWidget(showCrosshairCheckBox_ = new QCheckBox("Crosshair:"));
	chl->addSpacing(20);
	chl->addWidget(new QLabel("Color:"));
	chl->addWidget(crosshairColorPicker_ = new AMColorPickerButton(Qt::red));
	chl->addWidget(new QLabel("Line:"));
	chl->addWidget(crosshairThicknessSlider_ = new QSlider(Qt::Horizontal));
	crosshairThicknessSlider_->setMaximumWidth(80);
	crosshairThicknessSlider_->setRange(1,6);
	crosshairThicknessSlider_->setValue(1);
	chl->addSpacing(20);
	chl->addWidget(lockCrosshairCheckBox_ = new QCheckBox("Lock position"));
	chl->addStretch();
	crosshairFrame->setLayout(chl);

	vl->addWidget(crosshairFrame);
	vl->addWidget(videoWidget_ = new AMCrosshairOverlayVideoWidget(0, useOpenGlViewport));
	setLayout(vl);

	// Make conections:
	//////////////////////////

	connect(videoWidget_, SIGNAL(mouseDoubleClicked(QPointF)), this, SLOT(onVideoWidgetDoubleClicked(QPointF)));

	connect(crosshairColorPicker_, SIGNAL(colorChanged(QColor)), this, SLOT(setCrosshairColor(QColor)));
	connect(showCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairVisible(bool)));
	connect(lockCrosshairCheckBox_, SIGNAL(clicked(bool)), this, SLOT(setCrosshairLocked(bool)));
	connect(crosshairThicknessSlider_, SIGNAL(valueChanged(int)), this, SLOT(setCrosshairLineThickness(int)));

}


void AMBeamlineCameraWidget::setCrosshairColor(const QColor &color)
{
	QPen pen = videoWidget_->crosshairPen();
	pen.setColor(color);
	videoWidget_->setCrosshairPen(pen);

	if(crosshairColorPicker_->color() != color) {
		crosshairColorPicker_->blockSignals(true);
		crosshairColorPicker_->setColor(color);
		crosshairColorPicker_->blockSignals(false);
	}
}

void AMBeamlineCameraWidget::setCrosshairVisible(bool isVisible)
{
	videoWidget_->setCrosshairVisible(isVisible);

	if(showCrosshairCheckBox_->isChecked() != isVisible) {
		showCrosshairCheckBox_->blockSignals(true);
		showCrosshairCheckBox_->setChecked(isVisible);
		showCrosshairCheckBox_->blockSignals(false);
	}
}
void AMBeamlineCameraWidget::setCrosshairLocked(bool doLock)
{
	crosshairLocked_ = doLock;

	if(lockCrosshairCheckBox_->isChecked() != doLock) {
		lockCrosshairCheckBox_->blockSignals(true);
		lockCrosshairCheckBox_->setChecked(doLock);
		lockCrosshairCheckBox_->blockSignals(false);
	}
}

void AMBeamlineCameraWidget::onVideoWidgetDoubleClicked(const QPointF &clickPoint)
{
	if(!crosshairLocked_)
		videoWidget_->setCrosshairPosition(clickPoint);
}

QColor AMBeamlineCameraWidget::crosshairColor() const
{
	return videoWidget_->crosshairPen().color();
}

bool AMBeamlineCameraWidget::crosshairVisible() const
{
	return videoWidget_->crosshairVisible();
}

QPointF AMBeamlineCameraWidget::crosshairPosition() const
{
	return videoWidget_->crosshairPosition();
}

void AMBeamlineCameraWidget::setCrosshairPosition(const QPointF &pos) const
{
	videoWidget_->setCrosshairPosition(pos);
}

void AMBeamlineCameraWidget::setCrosshairLineThickness(int thickness)
{
	QPen pen = videoWidget_->crosshairPen();
	pen.setWidth(thickness);
	videoWidget_->setCrosshairPen(pen);

	if(crosshairThicknessSlider_->value() != thickness) {
		crosshairThicknessSlider_->blockSignals(true);
		crosshairThicknessSlider_->setValue(thickness);
		crosshairThicknessSlider_->blockSignals(false);
	}
}

int AMBeamlineCameraWidget::crosshairLineThickness() const
{
	return videoWidget_->crosshairPen().width();
}

void AMBeamlineCameraWidget::playSource(const QUrl& sourceUrl)
{
	videoWidget_->mediaPlayer()->setMedia(sourceUrl);
	videoWidget_->mediaPlayer()->play();
}


bool AMBeamlineCameraWidget::playSource(const QString& sourceUrl)
{
	QUrl url = QUrl::fromUserInput(sourceUrl);

	if(!url.isValid())
		return false;

	playSource(url);
	return true;
}

QUrl AMBeamlineCameraWidget::source() const
{
	return videoWidget_->mediaPlayer()->media().canonicalUrl();
}
