#include "CLSCrossHairGeneratorControlView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

CLSCrossHairGeneratorControlView::CLSCrossHairGeneratorControlView(CLSCrossHairGeneratorControl *crossHairGenerator, QWidget *parent)
	: QWidget(parent)
{
	crossHairGenerator_ = crossHairGenerator;

	channel_ = new QComboBox;

	for (int i = 0; i < 8; i++)
		channel_->addItem(QString("Channel %1").arg(i+1));

	channel_->setCurrentIndex(crossHairGenerator_->channel());

	horizontalPosition1_ = new QSpinBox;
	horizontalPosition1_->setRange(1, 479);
	horizontalPosition1_->setValue(crossHairGenerator_->horizontalPosition1());

	horizontalSlider1_ = new QSlider;
	horizontalSlider1_->setInvertedAppearance(true);
	horizontalSlider1_->setRange(1, 479);
	horizontalSlider1_->setValue(crossHairGenerator_->horizontalPosition1());

	horizontalPosition2_ = new QSpinBox;
	horizontalPosition2_->setRange(1, 479);
	horizontalPosition2_->setValue(crossHairGenerator_->horizontalPosition2());

	horizontalSlider2_ = new QSlider;
	horizontalSlider2_->setInvertedAppearance(true);
	horizontalSlider2_->setRange(1, 479);
	horizontalSlider2_->setValue(crossHairGenerator_->horizontalPosition2());

	verticalPosition1_ = new QSpinBox;
	verticalPosition1_->setRange(1, 639);
	verticalPosition1_->setValue(crossHairGenerator_->verticalPosition1());

	verticalSlider1_ = new QSlider;
	verticalSlider1_->setOrientation(Qt::Horizontal);
	verticalSlider1_->setRange(1, 479);
	verticalSlider1_->setValue(crossHairGenerator_->verticalPosition1());

	verticalPosition2_ = new QSpinBox;
	verticalPosition2_->setRange(1, 639);
	verticalPosition2_->setValue(crossHairGenerator_->verticalPosition2());

	verticalSlider2_ = new QSlider;
	verticalSlider2_->setOrientation(Qt::Horizontal);
	verticalSlider2_->setRange(1, 479);
	verticalSlider2_->setValue(crossHairGenerator_->horizontalPosition2());

	horizontalType1_ = new QComboBox;

	for (int i = 0; i < 16; i++)
		horizontalType1_->addItem(QString("Type %1").arg(i));

	horizontalType1_->setCurrentIndex(crossHairGenerator_->horizontalType1());

	horizontalType2_ = new QComboBox;

	for (int i = 0; i < 16; i++)
		horizontalType2_->addItem(QString("Type %1").arg(i));

	horizontalType2_->setCurrentIndex(crossHairGenerator_->horizontalType2());

	verticalType1_ = new QComboBox;

	for (int i = 0; i < 16; i++)
		verticalType1_->addItem(QString("Type %1").arg(i));

	verticalType1_->setCurrentIndex(crossHairGenerator_->verticalType1());

	verticalType2_ = new QComboBox;

	for (int i = 0; i < 16; i++)
		verticalType2_->addItem(QString("Type %1").arg(i));

	verticalType2_->setCurrentIndex(crossHairGenerator_->verticalType2());

	displayButton_ = new QPushButton;
	displayButton_->setCheckable(true);
	displayButton_->setChecked(crossHairGenerator_->displayState());

	boxButton_ = new QPushButton;
	boxButton_->setCheckable(true);
	boxButton_->setChecked(crossHairGenerator_->boxState());

	intensity_ = new QSlider;
	intensity_->setOrientation(Qt::Horizontal);
	intensity_->setRange(0, 63);
	intensity_->setValue(crossHairGenerator_->intensity());

	connect(channel_, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelChanged(int)));

	connect(horizontalPosition1_, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalPosition1Changed(int)));
	connect(horizontalPosition2_, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalPosition2Changed(int)));
	connect(verticalPosition1_, SIGNAL(valueChanged(int)), this, SLOT(onVerticalPosition1Changed(int)));
	connect(verticalPosition2_, SIGNAL(valueChanged(int)), this, SLOT(onVerticalPosition2Changed(int)));
	connect(horizontalSlider1_, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalPosition1Changed(int)));
	connect(horizontalSlider2_, SIGNAL(valueChanged(int)), this, SLOT(onHorizontalPosition2Changed(int)));
	connect(verticalSlider1_, SIGNAL(valueChanged(int)), this, SLOT(onVerticalPosition1Changed(int)));
	connect(verticalSlider2_, SIGNAL(valueChanged(int)), this, SLOT(onVerticalPosition2Changed(int)));

	connect(horizontalType1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onHorizontalType1Changed(int)));
	connect(horizontalType2_, SIGNAL(currentIndexChanged(int)), this, SLOT(onHorizontalType2Changed(int)));
	connect(verticalType1_, SIGNAL(currentIndexChanged(int)), this, SLOT(onVerticalType1Changed(int)));
	connect(verticalType2_, SIGNAL(currentIndexChanged(int)), this, SLOT(onVerticalType2Changed(int)));

	connect(displayButton_, SIGNAL(toggled(bool)), this, SLOT(onDisplayStateChanged(bool)));
	connect(boxButton_, SIGNAL(toggled(bool)), this, SLOT(onBoxStateChanged(bool)));
	connect(intensity_, SIGNAL(valueChanged(int)), this, SLOT(onIntensityChanged(int)));

	connect(crossHairGenerator_, SIGNAL(channelChanged()), this, SLOT(updateChannelChanged()));

	connect(crossHairGenerator_, SIGNAL(horizontalPosition1Changed()), this, SLOT(updateHorizontalPosition1Changed()));
	connect(crossHairGenerator_, SIGNAL(horizontalPosition2Changed()), this, SLOT(updateHorizontalPosition2Changed()));
	connect(crossHairGenerator_, SIGNAL(verticalPosition1Changed()), this, SLOT(updateVerticalPosition1Changed()));
	connect(crossHairGenerator_, SIGNAL(verticalPosition2Changed()), this, SLOT(updateVerticalPosition2Changed()));

	connect(crossHairGenerator_, SIGNAL(horizontalType1Changed()), this, SLOT(updateHorizontalType1Changed()));
	connect(crossHairGenerator_, SIGNAL(horizontalType2Changed()), this, SLOT(updateHorizontalType2Changed()));
	connect(crossHairGenerator_, SIGNAL(verticalType1Changed()), this, SLOT(updateVerticalType1Changed()));
	connect(crossHairGenerator_, SIGNAL(verticalType2Changed()), this, SLOT(updateVerticalType2Changed()));

	connect(crossHairGenerator_, SIGNAL(displayStateChanged()), this, SLOT(updateDisplayStateChanged()));
	connect(crossHairGenerator_, SIGNAL(boxStateChanged()), this, SLOT(updateBoxStateChanged()));
	connect(crossHairGenerator_, SIGNAL(intensityChanged()), this, SLOT(updateIntensityChanged()));

	QVBoxLayout *horizontalPositionLayout1 = new QVBoxLayout;
	horizontalPositionLayout1->addWidget(horizontalSlider1_);
	horizontalPositionLayout1->addWidget(horizontalPosition1_);
	horizontalPositionLayout1->addWidget(horizontalType1_);

	QVBoxLayout *horizontalPositionLayout2 = new QVBoxLayout;
	horizontalPositionLayout2->addWidget(horizontalSlider2_);
	horizontalPositionLayout2->addWidget(horizontalPosition2_);
	horizontalPositionLayout2->addWidget(horizontalType2_);

	QHBoxLayout *verticalPositionLayout1 = new QHBoxLayout;
	verticalPositionLayout1->addWidget(verticalSlider1_);
	verticalPositionLayout1->addWidget(verticalPosition1_);
	verticalPositionLayout1->addWidget(verticalType1_);

	QHBoxLayout *verticalPositionLayout2 = new QHBoxLayout;
	verticalPositionLayout2->addWidget(verticalSlider2_);
	verticalPositionLayout2->addWidget(verticalPosition2_);
	verticalPositionLayout2->addWidget(verticalType2_);

	QVBoxLayout *displayLayout = new QVBoxLayout;
	displayLayout->addWidget(new QLabel("Display"));
	displayLayout->addWidget(displayButton_);

	QVBoxLayout *boxLayout = new QVBoxLayout;
	boxLayout->addWidget(new QLabel("Box Mode"));
	boxLayout->addWidget(boxButton_);

	QVBoxLayout *intensityLayout = new QVBoxLayout;
	intensityLayout->addWidget(new QLabel("Intensity"));
	intensityLayout->addWidget(intensity_);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addLayout(displayLayout);
	buttonLayout->addLayout(boxLayout);

	QVBoxLayout *leftSideLayout = new QVBoxLayout;
	leftSideLayout->addWidget(channel_);
	leftSideLayout->addLayout(verticalPositionLayout1);
	leftSideLayout->addLayout(verticalPositionLayout2);
	leftSideLayout->addLayout(buttonLayout);
	leftSideLayout->addLayout(intensityLayout);

	QHBoxLayout *rightSideLayout = new QHBoxLayout;
	rightSideLayout->addLayout(horizontalPositionLayout1);
	rightSideLayout->addLayout(horizontalPositionLayout2);

	QHBoxLayout *mainLayout = new QHBoxLayout;
	mainLayout->addLayout(leftSideLayout);
	mainLayout->addLayout(rightSideLayout);

	setLayout(mainLayout);
}

CLSCrossHairGeneratorControlView::~CLSCrossHairGeneratorControlView()
{

}

void CLSCrossHairGeneratorControlView::onChannelChanged(int index)
{
	crossHairGenerator_->setChannel(index);
}

void CLSCrossHairGeneratorControlView::onHorizontalPosition1Changed(int value)
{
	crossHairGenerator_->setHorizontalPosition1(value);
}

void CLSCrossHairGeneratorControlView::onHorizontalPosition2Changed(int value)
{
	crossHairGenerator_->setHorizontalPosition2(value);
}

void CLSCrossHairGeneratorControlView::onVerticalPosition1Changed(int value)
{
	crossHairGenerator_->setVerticalPosition1(value);
}

void CLSCrossHairGeneratorControlView::onVerticalPosition2Changed(int value)
{
	crossHairGenerator_->setVerticalPosition2(value);
}

void CLSCrossHairGeneratorControlView::onHorizontalType1Changed(int value)
{
	crossHairGenerator_->setHorizontalType1(value);
}

void CLSCrossHairGeneratorControlView::onHorizontalType2Changed(int value)
{
	crossHairGenerator_->setHorizontalType2(value);
}

void CLSCrossHairGeneratorControlView::onVerticalType1Changed(int value)
{
	crossHairGenerator_->setVerticalType1(value);
}

void CLSCrossHairGeneratorControlView::onVerticalType2Changed(int value)
{
	crossHairGenerator_->setVerticalType2(value);
}

void CLSCrossHairGeneratorControlView::onDisplayStateChanged(bool state)
{
	crossHairGenerator_->setDisplayState(state);
}

void CLSCrossHairGeneratorControlView::onBoxStateChanged(bool state)
{
	crossHairGenerator_->setBoxState(state);
}

void CLSCrossHairGeneratorControlView::onIntensityChanged(int value)
{
	crossHairGenerator_->setIntensity(value);
}

void CLSCrossHairGeneratorControlView::updateChannelChanged()
{
	channel_->blockSignals(true);
	channel_->setCurrentIndex(crossHairGenerator_->channel());
	channel_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateHorizontalPosition1Changed()
{
	horizontalPosition1_->blockSignals(true);
	horizontalPosition1_->setValue(crossHairGenerator_->horizontalPosition1());
	horizontalPosition1_->blockSignals(false);

	horizontalSlider1_->blockSignals(true);
	horizontalSlider1_->setValue(crossHairGenerator_->horizontalPosition1());
	horizontalSlider1_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateHorizontalPosition2Changed()
{
	horizontalPosition2_->blockSignals(true);
	horizontalPosition2_->setValue(crossHairGenerator_->horizontalPosition2());
	horizontalPosition2_->blockSignals(false);

	horizontalSlider2_->blockSignals(true);
	horizontalSlider2_->setValue(crossHairGenerator_->horizontalPosition2());
	horizontalSlider2_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateVerticalPosition1Changed()
{
	verticalPosition1_->blockSignals(true);
	verticalPosition1_->setValue(crossHairGenerator_->verticalPosition1());
	verticalPosition1_->blockSignals(false);

	verticalSlider1_->blockSignals(true);
	verticalSlider1_->setValue(crossHairGenerator_->verticalPosition1());
	verticalSlider1_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateVerticalPosition2Changed()
{
	verticalPosition2_->blockSignals(true);
	verticalPosition2_->setValue(crossHairGenerator_->verticalPosition2());
	verticalPosition2_->blockSignals(false);

	verticalSlider2_->blockSignals(true);
	verticalSlider2_->setValue(crossHairGenerator_->verticalPosition2());
	verticalSlider2_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateHorizontalType1Changed()
{
	horizontalType1_->blockSignals(true);
	horizontalType1_->setCurrentIndex(crossHairGenerator_->horizontalType1());
	horizontalType1_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateHorizontalType2Changed()
{
	horizontalType2_->blockSignals(true);
	horizontalType2_->setCurrentIndex(crossHairGenerator_->horizontalType2());
	horizontalType2_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateVerticalType1Changed()
{
	verticalType1_->blockSignals(true);
	verticalType1_->setCurrentIndex(crossHairGenerator_->verticalType1());
	verticalType1_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateVerticalType2Changed()
{
	verticalType2_->blockSignals(true);
	verticalType2_->setCurrentIndex(crossHairGenerator_->verticalType2());
	verticalType2_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateDisplayStateChanged()
{
	displayButton_->blockSignals(true);
	displayButton_->setChecked(crossHairGenerator_->displayState());
	displayButton_->setText(crossHairGenerator_->displayState() ? "On" : "Off");
	displayButton_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateBoxStateChanged()
{
	boxButton_->blockSignals(true);
	boxButton_->setChecked(crossHairGenerator_->boxState());
	boxButton_->setText(crossHairGenerator_->boxState() ? "On" : "Off");
	boxButton_->blockSignals(false);
}

void CLSCrossHairGeneratorControlView::updateIntensityChanged()
{
	intensity_->blockSignals(true);
	intensity_->setValue(crossHairGenerator_->intensity());
	intensity_->blockSignals(false);
}
