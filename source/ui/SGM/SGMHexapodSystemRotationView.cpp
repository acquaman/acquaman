#include "SGMHexapodSystemRotationView.h"
#include <QGridLayout>

#include <QPaintEvent>
#include <QPainter>
#include <QPoint>

#include "beamline/SGM/SGMHexapod.h"
SGMHexapodSystemRotationView::SGMHexapodSystemRotationView(SGMHexapod* hexapod, QWidget *parent) :
    QWidget(parent)
{
	hexapod_ = hexapod;
	setupUI();
}

void SGMHexapodSystemRotationView::onApplyButtonClicked()
{
	double xAngle = xAngleTransformSpinbox_->value();
	double yAngle = yAngleTransformSpinbox_->value();
	double zAngle = zAngleTransformSpinbox_->value();

	if(hexapod_) {
		hexapod_->rotateSystem(xAngle, yAngle, zAngle);
	}

	xAngleTransformSpinbox_->setValue(0);
	yAngleTransformSpinbox_->setValue(0);
	zAngleTransformSpinbox_->setValue(0);
}

void SGMHexapodSystemRotationView::onResetButtonClicked()
{
	if(hexapod_) {
		// Set back to plane of the base plate
		hexapod_->resetSystem();
		// Set to plane of the sample plate
		hexapod_->rotateSystem(45, 0, 105);
	}

	xAngleTransformSpinbox_->setValue(0);
	yAngleTransformSpinbox_->setValue(0);
	zAngleTransformSpinbox_->setValue(0);
}

void SGMHexapodSystemRotationView::setupUI()
{
	QGridLayout* mainLayout = new QGridLayout();
	setLayout(mainLayout);

	xAngleTransformLabel_ = new QLabel("X Angle Tweak");
	yAngleTransformLabel_ = new QLabel("Y Angle Tweak");
	zAngleTransformLabel_ = new QLabel("Z Angle Tweak");

	xAngleTransformSpinbox_ = new QDoubleSpinBox();
	xAngleTransformSpinbox_->setSuffix(" deg");
	xAngleTransformSpinbox_->setRange(-360, 360);

	yAngleTransformSpinbox_ = new QDoubleSpinBox();
	yAngleTransformSpinbox_->setSuffix(" deg");
	yAngleTransformSpinbox_->setRange(-360, 360);

	zAngleTransformSpinbox_ = new QDoubleSpinBox();
	zAngleTransformSpinbox_->setSuffix(" deg");
	zAngleTransformSpinbox_->setRange(-360, 360);

	applyRotationButton_ = new QPushButton("Apply");
	resetRotationButton_ = new QPushButton("Restore default");

	mainLayout->addWidget(xAngleTransformLabel_, 0, 0);
	mainLayout->addWidget(xAngleTransformSpinbox_, 0, 1);

	mainLayout->addWidget(yAngleTransformLabel_, 1, 0);
	mainLayout->addWidget(yAngleTransformSpinbox_, 1, 1);

	mainLayout->addWidget(zAngleTransformLabel_, 2, 0);
	mainLayout->addWidget(zAngleTransformSpinbox_, 2, 1);

	mainLayout->addWidget(applyRotationButton_, 3, 0);
	mainLayout->addWidget(resetRotationButton_, 3, 1);

	connect(applyRotationButton_, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()));
	connect(resetRotationButton_, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));
}


