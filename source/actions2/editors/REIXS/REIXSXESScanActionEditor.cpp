#include "REIXSXESScanActionEditor.h"

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialog>

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "ui/REIXS/REIXSXESScanConfigurationView.h"

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

	QPushButton* moreDetailsButton = new QPushButton("More...");

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Detector eV"));
	hl->addWidget(energy_);
	hl->addWidget(new QLabel("Defocus (mm)"));
	hl->addWidget(focusOffset_);
	hl->addWidget(new QLabel("Tilt offset (deg)"));
	hl->addWidget(tiltOffset_);
	hl->addStretch();
	hl->addWidget(moreDetailsButton);


	energy_->setValue(info_->xesConfig()->centerEV());
	tiltOffset_->setValue(info_->xesConfig()->detectorTiltOffset());
	focusOffset_->setValue(info_->xesConfig()->defocusDistanceMm());

	connect(energy_, SIGNAL(editingFinished()), this, SLOT(onEnergyEditingFinished()));
	connect(focusOffset_, SIGNAL(editingFinished()), this, SLOT(onFocusOffsetEditingFinished()));
	connect(tiltOffset_, SIGNAL(editingFinished()), this, SLOT(onTiltOffsetEditingFinished()));
	connect(moreDetailsButton, SIGNAL(clicked()), this, SLOT(onMoreDetailsButtonClicked()));

	connect(info_->xesConfig(), SIGNAL(configurationChanged()), this, SLOT(onScanConfigurationChanged()));
}

void REIXSXESScanActionEditor::onEnergyEditingFinished()
{
	// TODO: Down the road, would it make sense for scan configurations to be AMActionInfo objects themselves... so we don't need this wrapper layer?
	info_->xesConfig()->setCenterEV(energy_->value());
}

void REIXSXESScanActionEditor::onFocusOffsetEditingFinished()
{
	info_->xesConfig()->setDefocusDistanceMm(focusOffset_->value());
}

void REIXSXESScanActionEditor::onTiltOffsetEditingFinished()
{
	info_->xesConfig()->setDetectorTiltOffset(tiltOffset_->value());
}

void REIXSXESScanActionEditor::onMoreDetailsButtonClicked()
{
	REIXSXESScanConfigurationView* view = new REIXSXESScanConfigurationView(info_->xesConfig());
	QDialog* dialog = new QDialog(this, Qt::Tool);
	QVBoxLayout* vl = new QVBoxLayout(dialog);
	vl->addWidget(view);

	dialog->show();	// dialog is a child of this widget, so will be destroyed when this widget is destroyed.
}

void REIXSXESScanActionEditor::onScanConfigurationChanged()
{
	double energy = info_->xesConfig()->centerEV();
	double tilt = info_->xesConfig()->detectorTiltOffset();
	double defocus = info_->xesConfig()->defocusDistanceMm();

	if(energy_->value() != energy) {
		energy_->blockSignals(true);
		energy_->setValue(energy);
		energy_->blockSignals(false);
	}
	if(tiltOffset_->value() != tilt) {
		tiltOffset_->blockSignals(true);
		tiltOffset_->setValue(tilt);
		tiltOffset_->blockSignals(false);
	}
	if(focusOffset_->value() != defocus) {
		focusOffset_->blockSignals(true);
		focusOffset_->setValue(defocus);
		focusOffset_->blockSignals(false);
	}
}
