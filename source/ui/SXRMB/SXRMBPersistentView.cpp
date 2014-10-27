#include "SXRMBPersistentView.h"

#include <QBoxLayout>
#include <QGroupBox>

#include "beamline/SXRMB/SXRMBBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

SXRMBPersistentView::SXRMBPersistentView(QWidget *parent) :
	QWidget(parent)
{
	mainGroupBox_ = new QGroupBox("SXRMB Beamline");
	mainGroupBox_->setMinimumWidth(400);
	mainGroupBoxVL_ = new QVBoxLayout();
	mainGroupBoxVL_->addWidget(mainGroupBox_);
	setLayout(mainGroupBoxVL_);

	energyControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->energy());
	statusControlEditor_ = new AMExtendedControlEditor(SXRMBBeamline::sxrmb()->beamlineStatus(), 0, true);

	mainVL_ = new QVBoxLayout();
	mainVL_->setContentsMargins(2, 2, 2, 2);

	mainVL_->addWidget(statusControlEditor_);
	mainVL_->addWidget(energyControlEditor_);
	mainVL_->addStretch();

	mainGroupBox_->setLayout(mainVL_);
}

SXRMBPersistentView::~SXRMBPersistentView()
{
}
