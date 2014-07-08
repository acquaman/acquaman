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


#include "AMImagePropertyEditor.h"
#include "ui_AMImagePropertyEditor.h"

AMImagePropertyEditor::AMImagePropertyEditor(QWidget *parent) :
	QFrame(parent),
	ui_(new Ui::AMImagePropertyEditor),
	currentMap_(MPlotColorMap::Jet)
{
	ui_->setupUi(this);
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(ui_->useStandardColorMapButton);
	typeBG->addButton(ui_->useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(ui_->rgbButton, 0);
	blendModeBG->addButton(ui_->hsvButton, 1);
	blendModeBG->setExclusive(true);

	brightness_ = 0;
	contrast_ = 1;
	gamma_ = 1;

	ui_->secondColorButton->setColor(Qt::white);

	makeConnections();
}

void AMImagePropertyEditor::makeConnections()
{
	connect(ui_->useStandardColorMapButton, SIGNAL(toggled(bool)), this, SLOT(onUseStandardColorMapChanged(bool)));
	connect(ui_->standardColorMapComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardColorMapChanged(int)));

	connect(ui_->useStandardColorMapButton, SIGNAL(toggled(bool)), ui_->standardColorMapComboBox, SLOT(setEnabled(bool)));
	connect(ui_->useCustomColorsButton, SIGNAL(toggled(bool)), ui_->firstColorButton, SLOT(setEnabled(bool)));
	connect(ui_->useCustomColorsButton, SIGNAL(toggled(bool)), ui_->secondColorButton, SLOT(setEnabled(bool)));
	connect(ui_->useCustomColorsButton, SIGNAL(toggled(bool)), ui_->rgbButton, SLOT(setEnabled(bool)));
	connect(ui_->useCustomColorsButton, SIGNAL(toggled(bool)), ui_->hsvButton, SLOT(setEnabled(bool)));

	connect(ui_->firstColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onFirstColorChanged(QColor)));
	connect(ui_->secondColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onSecondColorChanged(QColor)));
	connect(ui_->hsvButton, SIGNAL(toggled(bool)), this, SLOT(onBlendModeChanged(bool)));

	connect(ui_->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(onBrightnessSliderChanged(int)));
	connect(ui_->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(onContrastSliderChanged(int)));
	connect(ui_->gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(onGammaSliderChanged(int)));
}

AMImagePropertyEditor::AMImagePropertyEditor(bool useStandardColorMap, int mplotStandardColorMap, const QColor& firstColor, const QColor& secondColor, double brightness, double contrast, double gamma, QWidget* parent) :
	QFrame(parent),
	ui_(new Ui::AMImagePropertyEditor)
{
	ui_->setupUi(this);
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(ui_->useStandardColorMapButton);
	typeBG->addButton(ui_->useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(ui_->rgbButton, 0);
	blendModeBG->addButton(ui_->hsvButton, 1);
	blendModeBG->setExclusive(true);

	brightness_ = brightness;
	contrast_ = contrast;
	gamma_ = gamma;

	ui_->useStandardColorMapButton->setChecked(useStandardColorMap);
	ui_->useCustomColorsButton->setChecked(!useStandardColorMap);

	if(mplotStandardColorMap != -1)
		ui_->standardColorMapComboBox->setCurrentIndex(qBound(0, mplotStandardColorMap, 12));
	else
		ui_->standardColorMapComboBox->setCurrentIndex(7);// default (unselected): Jet.  todo: make defaults configurable?

	ui_->firstColorButton->setColor(firstColor);
	ui_->secondColorButton->setColor(secondColor);

	int brightnessSliderPos, contrastSliderPos, gammaSliderPos;
	brightnessSliderPos = qBound(-100, int(brightness_*400), 100);
	if(contrast_>=1)
		contrastSliderPos = qBound(-100, int((contrast_ - 1.0)*100./4.), 100);
	else
		contrastSliderPos = qBound(-100, int((1.0 - 1.0/contrast_)*100./4.), 100);
	if(gamma_>=1)
		gammaSliderPos = qBound(-100, int((gamma_ - 1.0)*100./2.), 100);
	else
		gammaSliderPos = qBound(-100, int((1.0 - 1.0/gamma_)*100./2.), 100);

	ui_->brightnessSlider->setValue(brightnessSliderPos);
	ui_->contrastSlider->setValue(contrastSliderPos);
	ui_->gammaSlider->setValue(gammaSliderPos);


	ui_->brightnessText->setText(QString::number(brightness_, 'f', 2));
	ui_->contrastText->setText(QString::number(contrast_, 'f', 2));
	ui_->gammaText->setText(QString::number(gamma_, 'f', 2));


	if(!useStandardColorMap) {
		ui_->standardColorMapComboBox->setEnabled(false);
		ui_->firstColorButton->setEnabled(true);
		ui_->secondColorButton->setEnabled(true);
		ui_->rgbButton->setEnabled(true);
		ui_->hsvButton->setEnabled(true);
	}

	// setup current color map
	if(useStandardColorMap)
		currentMap_ = MPlotColorMap((MPlotColorMap::StandardColorMap)qBound(0, mplotStandardColorMap, 12));
	else
		currentMap_ = MPlotColorMap(firstColor, secondColor);

	currentMap_.setBrightness(brightness_);
	currentMap_.setContrast(contrast_);
	currentMap_.setGamma(gamma_);

	makeConnections();

}

AMImagePropertyEditor::AMImagePropertyEditor(const MPlotColorMap& colorMap, QWidget* parent) :
	QFrame(parent),
	ui_(new Ui::AMImagePropertyEditor)
{
	brightness_ = colorMap.brightness();
	contrast_ = colorMap.contrast();
	gamma_ = colorMap.gamma();


	ui_->setupUi(this);
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(ui_->useStandardColorMapButton);
	typeBG->addButton(ui_->useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(ui_->rgbButton, 0);
	blendModeBG->addButton(ui_->hsvButton, 1);
	blendModeBG->setExclusive(true);

	ui_->useStandardColorMapButton->setChecked(colorMap.standardColorMapValue() != -1);
	ui_->useCustomColorsButton->setChecked(colorMap.standardColorMapValue() == -1);

	if(colorMap.standardColorMapValue() != -1)
		ui_->standardColorMapComboBox->setCurrentIndex(qBound(0, colorMap.standardColorMapValue(), 12));
	else
		ui_->standardColorMapComboBox->setCurrentIndex(7);// default (unselected): Jet.  todo: make defaults configurable?

	QGradientStops stops = colorMap.stops();
	if(stops.count() >= 2) {
		ui_->firstColorButton->setColor(stops.first().second);
		ui_->secondColorButton->setColor(stops.last().second);
	}
	else {
		ui_->firstColorButton->setColor(Qt::black);
		ui_->secondColorButton->setColor(Qt::white);
	}
	if(colorMap.blendMode() == MPlotColorMap::RGB)
		ui_->rgbButton->setChecked(true);
	else
		ui_->hsvButton->setChecked(true);

	int brightnessSliderPos, contrastSliderPos, gammaSliderPos;
	brightnessSliderPos = qBound(-100, int(brightness_*400), 100);
	if(contrast_>=1)
		contrastSliderPos = qBound(-100, int((contrast_ - 1.0)*100./4.), 100);
	else
		contrastSliderPos = qBound(-100, int((1.0 - 1.0/contrast_)*100./4.), 100);
	if(gamma_>=1)
		gammaSliderPos = qBound(-100, int((gamma_ - 1.0)*100./2.), 100);
	else
		gammaSliderPos = qBound(-100, int((1.0 - 1.0/gamma_)*100./2.), 100);

	ui_->brightnessSlider->setValue(brightnessSliderPos);
	ui_->contrastSlider->setValue(contrastSliderPos);
	ui_->gammaSlider->setValue(gammaSliderPos);

	ui_->brightnessText->setText(QString::number(brightness_, 'f', 2));
	ui_->contrastText->setText(QString::number(contrast_, 'f', 2));
	ui_->gammaText->setText(QString::number(gamma_, 'f', 2));


	if(!useStandardColorMap()) {
		ui_->standardColorMapComboBox->setEnabled(false);
		ui_->firstColorButton->setEnabled(true);
		ui_->secondColorButton->setEnabled(true);
		ui_->rgbButton->setEnabled(true);
		ui_->hsvButton->setEnabled(true);
	}

	// setup current color map
	currentMap_ = colorMap;

	makeConnections();

}


AMImagePropertyEditor::~AMImagePropertyEditor()
{
	delete ui_;
}

void AMImagePropertyEditor::onBrightnessSliderChanged(int sliderValue)
{
	// slider value from -100 to 100. Map to -.25 to +.25
	brightness_ = double(sliderValue)/400;
	ui_->brightnessText->setText(QString::number(brightness_, 'f', 2));
	currentMap_.setBrightness(brightness_);

	emit brightnessChanged(brightness_);
	emit colorMapChanged(currentMap_);

}

void AMImagePropertyEditor::onContrastSliderChanged(int sliderValue)
{
	// slider value from -100 to 100.  Map negatives from 0.1 to 1; positives from 1 to 10
	if(sliderValue >= 0)
		contrast_ = 1.0 + double(sliderValue)*4./100.;	// positive scale factor.

	else
		contrast_ = 1.0/(1.0 - double(sliderValue)*4./100.);	// if negative: invert and make positive

	ui_->contrastText->setText(QString::number(contrast_, 'f', 2));
	currentMap_.setContrast(contrast_);

	emit contrastChanged(contrast_);
	emit colorMapChanged(currentMap_);

}

void AMImagePropertyEditor::onGammaSliderChanged(int sliderValue)
{
	// slider value from -100 to 100. Map negatives from 0.25 to 1; positives from 1 to 4
	if(sliderValue >= 0)
		gamma_ = 1.0 + double(sliderValue)*2.0/100.0;

	else
		gamma_ = 1.0/(1.0 - double(sliderValue)*2.0/100.0);

	ui_->gammaText->setText(QString::number(gamma_, 'f', 2));
	currentMap_.setGamma(gamma_);

	emit gammaChanged(gamma_);
	emit colorMapChanged(currentMap_);

}

bool AMImagePropertyEditor::useStandardColorMap() const
{
	return ui_->useStandardColorMapButton->isChecked();
}

int AMImagePropertyEditor::standardColorMap() const
{
	return qBound(0, ui_->standardColorMapComboBox->currentIndex(), 12);
}

QColor AMImagePropertyEditor::firstColor() const
{
	return ui_->firstColorButton->color();
}

QColor AMImagePropertyEditor::secondColor() const
{
	return ui_->secondColorButton->color();
}


void AMImagePropertyEditor::onUseStandardColorMapChanged(bool useStandard)
{
	if(useStandard)
		currentMap_ = MPlotColorMap((MPlotColorMap::StandardColorMap)standardColorMap());
	else {
		currentMap_ = MPlotColorMap(firstColor(), secondColor());
		currentMap_.setBlendMode(blendMode());
	}

	currentMap_.setBrightness(brightness_);
	currentMap_.setContrast(contrast_);
	currentMap_.setGamma(gamma_);

	emit useStandardColorMapChanged(useStandard);
	emit colorMapChanged(currentMap_);
}

void AMImagePropertyEditor::onStandardColorMapChanged(int index)
{
	Q_UNUSED(index)

	if(!useStandardColorMap())
		return;

	MPlotColorMap::StandardColorMap sm = (MPlotColorMap::StandardColorMap)standardColorMap();
	currentMap_ = MPlotColorMap(sm);
	currentMap_.setBrightness(brightness_);
	currentMap_.setContrast(contrast_);
	currentMap_.setGamma(gamma_);

	emit standardColorMapChanged(sm);
	emit colorMapChanged(currentMap_);
}

void AMImagePropertyEditor::onFirstColorChanged(const QColor &color)
{
	if(useStandardColorMap())
		return;

	currentMap_.setStops(QGradientStops() << QGradientStop(0., color) << QGradientStop(1., secondColor()));

	emit firstColorChanged(color);
	emit colorMapChanged(currentMap_);
}

void AMImagePropertyEditor::onSecondColorChanged(const QColor &color)
{
	if(useStandardColorMap())
		return;

	currentMap_.setStops(QGradientStops() << QGradientStop(0., firstColor()) << QGradientStop(1., color));

	emit secondColorChanged(color);
	emit colorMapChanged(currentMap_);
}

MPlotColorMap::BlendMode AMImagePropertyEditor::blendMode() const
{
	if(ui_->hsvButton->isChecked())
		return MPlotColorMap::HSV;
	else
		return MPlotColorMap::RGB;
}

void AMImagePropertyEditor::onBlendModeChanged(bool blendModeIsHSV)
{
	if(useStandardColorMap())
		return;

	if(blendModeIsHSV)
		currentMap_.setBlendMode(MPlotColorMap::HSV);
	else
		currentMap_.setBlendMode(MPlotColorMap::RGB);

	emit blendModeChanged(blendModeIsHSV);
	emit colorMapChanged(currentMap_);
}


