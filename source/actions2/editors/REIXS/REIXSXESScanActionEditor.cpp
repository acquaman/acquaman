#include "REIXSXESScanActionEditor.h"

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

REIXSXESScanActionEditor::REIXSXESScanActionEditor(REIXSXESScanActionInfo *info, QWidget *parent)
	: QFrame(parent) {

	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	energy_ = new QDoubleSpinBox();
	energy_->setDecimals(3);
	energy_->setRange(50, 1400);	// todo: restrict to current grating?

	focusOffset_ = new QDoubleSpinBox();
	focusOffset_->setDecimals(2);
	focusOffset_->setRange(-1000, 1000);

	tiltOffset_ = new QDoubleSpinBox();
	tiltOffset_->setDecimals(2);
	tiltOffset_->setRange(-6, 6);
	tiltOffset_->setSingleStep(0.1);

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Detector eV"));
	hl->addWidget(energy_);
	hl->addWidget(new QLabel("Defocus (mm)"));
	hl->addWidget(focusOffset_);
	hl->addWidget(new QLabel("Tilt offset (deg)"));
	hl->addWidget(tiltOffset_);
	hl->addStretch();

	energy_->setValue(info_->configCenterEV());
	tiltOffset_->setValue(info_->configTiltOffset());
	focusOffset_->setValue(info_->configDefocusMm());

	connect(energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyEditingFinished()));
	connect(focusOffset_, SIGNAL(editingFinished()), this, SLOT(onFocusOffsetEditingFinished()));
	connect(tiltOffset_, SIGNAL(editingFinished()), this, SLOT(onTiltOffsetEditingFinished()));
}

void REIXSXESScanActionEditor::onEnergyEditingFinished()
{
	// TODO: Down the road, would it make sense for scan configurations to be AMActionInfo objects themselves... so we don't need this wrapper layer?
	info_->setConfigCenterEV(energy_->value());
}

void REIXSXESScanActionEditor::onFocusOffsetEditingFinished()
{
	info_->setConfigDefocusMm(focusOffset_->value());
}

void REIXSXESScanActionEditor::onTiltOffsetEditingFinished()
{
	info_->setConfigTiltOffset(tiltOffset_->value());
}
