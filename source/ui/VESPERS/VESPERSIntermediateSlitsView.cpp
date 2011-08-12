#include "VESPERSIntermediateSlitsView.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QVBoxLayout>

VESPERSIntermediateSlitsView::VESPERSIntermediateSlitsView(VESPERSIntermediateSlits *slits, QWidget *parent)
	: QWidget(parent)
{
	slits_ = slits;

	xSlit_ = new QDoubleSpinBox;
	xSlit_->setSuffix(" mm");
	xSlit_->setDecimals(3);
	xSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapXChanged(double)), xSlit_, SLOT(setValue(double)));
	connect(xSlit_, SIGNAL(editingFinished()), this, SLOT(setXGap()));

	zSlit_ = new QDoubleSpinBox;
	zSlit_->setSuffix(" mm");
	zSlit_->setDecimals(3);
	zSlit_->setSingleStep(0.001);
	connect(slits_, SIGNAL(gapZChanged(double)), zSlit_, SLOT(setValue(double)));
	connect(zSlit_, SIGNAL(editingFinished()), this, SLOT(setZGap()));

	QFormLayout *layout = new QFormLayout;
	layout->addRow("X-slit gap:", xSlit_);
	layout->addRow("Z-slit gap", zSlit_);

	QGroupBox *slitGroupBox = new QGroupBox("Intermediate Slits");
	slitGroupBox->setLayout(layout);

	QVBoxLayout *slitLayout = new QVBoxLayout;
	slitLayout->addWidget(slitGroupBox);

	setLayout(slitLayout);
}
