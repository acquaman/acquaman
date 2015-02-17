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
#include "AMColorPickerButton.h"

AMImagePropertyEditor::AMImagePropertyEditor(QWidget *parent) :
	QFrame(parent),
	currentMap_(MPlotColorMap::Jet)
{
	setupUi();
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(useStandardColorMapButton);
	typeBG->addButton(useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(rgbButton, 0);
	blendModeBG->addButton(hsvButton, 1);
	blendModeBG->setExclusive(true);

	brightness_ = 0;
	contrast_ = 1;
	gamma_ = 1;

	secondColorButton->setColor(Qt::white);

	makeConnections();
}

void AMImagePropertyEditor::makeConnections()
{
	connect(useStandardColorMapButton, SIGNAL(toggled(bool)), this, SLOT(onUseStandardColorMapChanged(bool)));
	connect(standardColorMapComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardColorMapChanged(int)));

	connect(useStandardColorMapButton, SIGNAL(toggled(bool)), standardColorMapComboBox, SLOT(setEnabled(bool)));
	connect(useCustomColorsButton, SIGNAL(toggled(bool)), firstColorButton, SLOT(setEnabled(bool)));
	connect(useCustomColorsButton, SIGNAL(toggled(bool)), secondColorButton, SLOT(setEnabled(bool)));
	connect(useCustomColorsButton, SIGNAL(toggled(bool)), rgbButton, SLOT(setEnabled(bool)));
	connect(useCustomColorsButton, SIGNAL(toggled(bool)), hsvButton, SLOT(setEnabled(bool)));

	connect(firstColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onFirstColorChanged(QColor)));
	connect(secondColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onSecondColorChanged(QColor)));
	connect(hsvButton, SIGNAL(toggled(bool)), this, SLOT(onBlendModeChanged(bool)));

	connect(brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(onBrightnessSliderChanged(int)));
	connect(contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(onContrastSliderChanged(int)));
	connect(gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(onGammaSliderChanged(int)));
}

AMImagePropertyEditor::AMImagePropertyEditor(bool useStandardColorMap, int mplotStandardColorMap, const QColor& firstColor, const QColor& secondColor, double brightness, double contrast, double gamma, QWidget* parent) :
	QFrame(parent)
{
	setupUi();
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(useStandardColorMapButton);
	typeBG->addButton(useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(rgbButton, 0);
	blendModeBG->addButton(hsvButton, 1);
	blendModeBG->setExclusive(true);

	brightness_ = brightness;
	contrast_ = contrast;
	gamma_ = gamma;

	useStandardColorMapButton->setChecked(useStandardColorMap);
	useCustomColorsButton->setChecked(!useStandardColorMap);

	if(mplotStandardColorMap != -1)
		standardColorMapComboBox->setCurrentIndex(qBound(0, mplotStandardColorMap, 12));
	else
		standardColorMapComboBox->setCurrentIndex(7);// default (unselected): Jet.  todo: make defaults configurable?

	firstColorButton->setColor(firstColor);
	secondColorButton->setColor(secondColor);

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

	brightnessSlider->setValue(brightnessSliderPos);
	contrastSlider->setValue(contrastSliderPos);
	gammaSlider->setValue(gammaSliderPos);


	brightnessText->setText(QString::number(brightness_, 'f', 2));
	contrastText->setText(QString::number(contrast_, 'f', 2));
	gammaText->setText(QString::number(gamma_, 'f', 2));


	if(!useStandardColorMap) {
		standardColorMapComboBox->setEnabled(false);
		firstColorButton->setEnabled(true);
		secondColorButton->setEnabled(true);
		rgbButton->setEnabled(true);
		hsvButton->setEnabled(true);
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
	QFrame(parent)
{
	brightness_ = colorMap.brightness();
	contrast_ = colorMap.contrast();
	gamma_ = colorMap.gamma();


	setupUi();
	QButtonGroup* typeBG = new QButtonGroup(this);
	typeBG->addButton(useStandardColorMapButton);
	typeBG->addButton(useCustomColorsButton);
	typeBG->setExclusive(true);

	QButtonGroup* blendModeBG = new QButtonGroup(this);
	blendModeBG->addButton(rgbButton, 0);
	blendModeBG->addButton(hsvButton, 1);
	blendModeBG->setExclusive(true);

	useStandardColorMapButton->setChecked(colorMap.standardColorMapValue() != -1);
	useCustomColorsButton->setChecked(colorMap.standardColorMapValue() == -1);

	if(colorMap.standardColorMapValue() != -1)
		standardColorMapComboBox->setCurrentIndex(qBound(0, colorMap.standardColorMapValue(), 12));
	else
		standardColorMapComboBox->setCurrentIndex(7);// default (unselected): Jet.  todo: make defaults configurable?

	QGradientStops stops = colorMap.stops();
	if(stops.count() >= 2) {
		firstColorButton->setColor(stops.first().second);
		secondColorButton->setColor(stops.last().second);
	}
	else {
		firstColorButton->setColor(Qt::black);
		secondColorButton->setColor(Qt::white);
	}
	if(colorMap.blendMode() == MPlotColorMap::RGB)
		rgbButton->setChecked(true);
	else
		hsvButton->setChecked(true);

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

	brightnessSlider->setValue(brightnessSliderPos);
	contrastSlider->setValue(contrastSliderPos);
	gammaSlider->setValue(gammaSliderPos);

	brightnessText->setText(QString::number(brightness_, 'f', 2));
	contrastText->setText(QString::number(contrast_, 'f', 2));
	gammaText->setText(QString::number(gamma_, 'f', 2));


	if(!useStandardColorMap()) {
		standardColorMapComboBox->setEnabled(false);
		firstColorButton->setEnabled(true);
		secondColorButton->setEnabled(true);
		rgbButton->setEnabled(true);
		hsvButton->setEnabled(true);
	}

	// setup current color map
	currentMap_ = colorMap;

	makeConnections();

}

void AMImagePropertyEditor::onBrightnessSliderChanged(int sliderValue)
{
	// slider value from -100 to 100. Map to -.25 to +.25
	brightness_ = double(sliderValue)/400;
	brightnessText->setText(QString::number(brightness_, 'f', 2));
	currentMap_.setBrightness(brightness_);

	emit brightnessChanged(brightness_);
	emit colorMapChanged(currentMap_);

}

void AMImagePropertyEditor::onContrastSliderChanged(int sliderValue)
{
	// slider value from -100 to 100.  Map negatives from 0.1 to 1, positives from 1 to 10
	if(sliderValue >= 0)
		contrast_ = 1.0 + double(sliderValue)*4./100.;	// positive scale factor.

	else
		contrast_ = 1.0/(1.0 - double(sliderValue)*4./100.);	// if negative: invert and make positive

	contrastText->setText(QString::number(contrast_, 'f', 2));
	currentMap_.setContrast(contrast_);

	emit contrastChanged(contrast_);
	emit colorMapChanged(currentMap_);

}

void AMImagePropertyEditor::onGammaSliderChanged(int sliderValue)
{
	// slider value from -100 to 100. Map negatives from 0.25 to 1, positives from 1 to 4
	if(sliderValue >= 0)
		gamma_ = 1.0 + double(sliderValue)*2.0/100.0;

	else
		gamma_ = 1.0/(1.0 - double(sliderValue)*2.0/100.0);

	gammaText->setText(QString::number(gamma_, 'f', 2));
	currentMap_.setGamma(gamma_);

	emit gammaChanged(gamma_);
	emit colorMapChanged(currentMap_);

}

bool AMImagePropertyEditor::useStandardColorMap() const
{
	return useStandardColorMapButton->isChecked();
}

int AMImagePropertyEditor::standardColorMap() const
{
	return qBound(0, standardColorMapComboBox->currentIndex(), 12);
}

QColor AMImagePropertyEditor::firstColor() const
{
	return firstColorButton->color();
}

QColor AMImagePropertyEditor::secondColor() const
{
	return secondColorButton->color();
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
	if(hsvButton->isChecked())
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

void AMImagePropertyEditor::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMImagePropertyEditor"));
	resize(208, 275);
	setStyleSheet(QString::fromUtf8("#AMImagePropertyEditor {\n"
"	background-color: rgba(0, 0, 0, 179);\n"
"}"));
	setFrameShape(QFrame::NoFrame);
	setFrameShadow(QFrame::Plain);

	gridLayout = new QGridLayout();
	gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
	gridLayout->setContentsMargins(12, -1, 6, -1);

	useStandardColorMapButton = new QRadioButton();
	useStandardColorMapButton->setObjectName(QString::fromUtf8("useStandardColorMapButton"));
	useStandardColorMapButton->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
	useStandardColorMapButton->setChecked(true);
	useStandardColorMapButton->setAutoExclusive(false);

	gridLayout->addWidget(useStandardColorMapButton, 0, 0, 1, 4);

	standardColorMapComboBox = new QComboBox();
	standardColorMapComboBox->setObjectName(QString::fromUtf8("standardColorMapComboBox"));

	gridLayout->addWidget(standardColorMapComboBox, 1, 1, 1, 3);

	useCustomColorsButton = new QRadioButton();
	useCustomColorsButton->setObjectName(QString::fromUtf8("useCustomColorsButton"));
	useCustomColorsButton->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));
	useCustomColorsButton->setAutoExclusive(false);

	gridLayout->addWidget(useCustomColorsButton, 2, 0, 1, 3);

	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(18);
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

	firstColorButton = new AMColorPickerButton();
	firstColorButton->setObjectName(QString::fromUtf8("firstColorButton"));
	firstColorButton->setEnabled(false);

	horizontalLayout->addWidget(firstColorButton);

	secondColorButton = new AMColorPickerButton();
	secondColorButton->setObjectName(QString::fromUtf8("secondColorButton"));
	secondColorButton->setEnabled(false);

	horizontalLayout->addWidget(secondColorButton);

	verticalLayout = new QVBoxLayout();
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

	rgbButton = new QRadioButton();
	rgbButton->setObjectName(QString::fromUtf8("rgbButton"));
	rgbButton->setEnabled(false);
	rgbButton->setStyleSheet(QString::fromUtf8("color: rgb(230,230,230);"));
	rgbButton->setChecked(true);
	rgbButton->setAutoExclusive(false);

	verticalLayout->addWidget(rgbButton);

	hsvButton = new QRadioButton();
	hsvButton->setObjectName(QString::fromUtf8("hsvButton"));
	hsvButton->setEnabled(false);
	hsvButton->setStyleSheet(QString::fromUtf8("color: rgb(230,230,230);"));
	hsvButton->setAutoExclusive(false);

	verticalLayout->addWidget(hsvButton);


	horizontalLayout->addLayout(verticalLayout);


	gridLayout->addLayout(horizontalLayout, 3, 2, 1, 2);

	line = new QFrame();
	line->setObjectName(QString::fromUtf8("line"));
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);

	gridLayout->addWidget(line, 4, 0, 2, 4);

	brightnessText = new QLabel();
	brightnessText->setObjectName(QString::fromUtf8("brightnessText"));
	brightnessText->setMinimumSize(QSize(40, 0));
	brightnessText->setStyleSheet(QString::fromUtf8("color: rgb(230,230,230);"));
	brightnessText->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

	gridLayout->addWidget(brightnessText, 6, 3, 1, 1);

	labelBrightness = new QLabel();
	labelBrightness->setObjectName(QString::fromUtf8("labelBrightness"));
	labelBrightness->setPixmap(QPixmap(QString::fromUtf8(":/22x22/brightness.png")));

	gridLayout->addWidget(labelBrightness, 6, 0, 1, 1);

	brightnessSlider = new QSlider();
	brightnessSlider->setObjectName(QString::fromUtf8("brightnessSlider"));
	brightnessSlider->setMinimumSize(QSize(80, 0));
	brightnessSlider->setMinimum(-100);
	brightnessSlider->setMaximum(100);
	brightnessSlider->setValue(0);
	brightnessSlider->setOrientation(Qt::Horizontal);
	brightnessSlider->setInvertedAppearance(false);
	brightnessSlider->setTickPosition(QSlider::TicksBelow);
	brightnessSlider->setTickInterval(100);

	gridLayout->addWidget(brightnessSlider, 5, 2, 2, 1);

	labelContrast = new QLabel();
	labelContrast->setObjectName(QString::fromUtf8("labelContrast"));
	labelContrast->setPixmap(QPixmap(QString::fromUtf8(":/22x22/contrast.png")));

	gridLayout->addWidget(labelContrast, 7, 0, 1, 1);

	contrastSlider = new QSlider();
	contrastSlider->setObjectName(QString::fromUtf8("contrastSlider"));
	contrastSlider->setMinimumSize(QSize(80, 0));
	contrastSlider->setMinimum(-100);
	contrastSlider->setMaximum(100);
	contrastSlider->setValue(0);
	contrastSlider->setOrientation(Qt::Horizontal);
	contrastSlider->setTickPosition(QSlider::TicksBelow);
	contrastSlider->setTickInterval(100);

	gridLayout->addWidget(contrastSlider, 7, 2, 1, 1);

	contrastText = new QLabel();
	contrastText->setObjectName(QString::fromUtf8("contrastText"));
	contrastText->setMinimumSize(QSize(40, 0));
	contrastText->setStyleSheet(QString::fromUtf8("color: rgb(230,230,230);"));
	contrastText->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

	gridLayout->addWidget(contrastText, 7, 3, 1, 1);

	labelGamma = new QLabel();
	labelGamma->setObjectName(QString::fromUtf8("labelGamma"));
	labelGamma->setPixmap(QPixmap(QString::fromUtf8(":/22x22/gamma.png")));

	gridLayout->addWidget(labelGamma, 8, 0, 1, 1);

	gammaSlider = new QSlider();
	gammaSlider->setObjectName(QString::fromUtf8("gammaSlider"));
	gammaSlider->setMinimumSize(QSize(80, 0));
	gammaSlider->setMinimum(-100);
	gammaSlider->setMaximum(100);
	gammaSlider->setValue(0);
	gammaSlider->setOrientation(Qt::Horizontal);
	gammaSlider->setTickPosition(QSlider::TicksBelow);
	gammaSlider->setTickInterval(100);

	gridLayout->addWidget(gammaSlider, 8, 2, 1, 1);

	gammaText = new QLabel();
	gammaText->setObjectName(QString::fromUtf8("gammaText"));
	gammaText->setMinimumSize(QSize(40, 0));
	gammaText->setStyleSheet(QString::fromUtf8("color: rgb(230,230,230);"));
	gammaText->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

	gridLayout->addWidget(gammaText, 8, 3, 1, 1);

	setLayout(gridLayout);
}


