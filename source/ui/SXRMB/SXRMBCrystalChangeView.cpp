#include "SXRMBCrystalChangeView.h"

#include <QVBoxLayout>

SXRMBCrystalChangeView::SXRMBCrystalChangeView(SXRMBCrystalChangeModel *crystalChange, QWidget *parent)
	: QWidget(parent)
{
	crystalChange_ = crystalChange;

	connect(crystalChange_, SIGNAL(crystalSelectionChanged()), this, SLOT(onCrystalSelectionChanged()));

	QVBoxLayout *editorLayout = new QVBoxLayout;

	crystalSelectionEditor_ = new AMExtendedControlEditor(crystalChange_->crystalSelectionControl());
	editorLayout->addWidget(crystalSelectionEditor_);
	crystalYEditor_ = new AMExtendedControlEditor(crystalChange_->crystalYControl());
	crystalYEditor_->setControlFormat('f', 4);
	editorLayout->addWidget(crystalYEditor_);
	crystalThetaEditor_ = new AMExtendedControlEditor(crystalChange_->crystalThetaControl());
	crystalThetaEditor_->setControlFormat('f', 4);
	editorLayout->addWidget(crystalThetaEditor_);
	crystalChiEditor_ = new AMExtendedControlEditor(crystalChange_->crystalChiControl());
	crystalChiEditor_->setControlFormat('f', 4);
	editorLayout->addWidget(crystalChiEditor_);

	setLayout(editorLayout);
}

SXRMBCrystalChangeView::~SXRMBCrystalChangeView()
{

}

void SXRMBCrystalChangeView::onCrystalSelectionChanged()
{
	bool isInBetween = crystalChange_->isInBetween();
	crystalYEditor_->setDisabled(isInBetween);
	crystalThetaEditor_->setDisabled(isInBetween);
	crystalChiEditor_->setDisabled(isInBetween);
}
