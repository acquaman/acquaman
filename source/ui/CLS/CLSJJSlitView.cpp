#include "CLSJJSlitView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

#include "beamline/CLS/CLSJJSlit.h"
#include "ui/beamline/AMExtendedControlEditor.h"

CLSJJSlitView::CLSJJSlitView(CLSJJSlit *clsJJSlit, QWidget *parent) :
    QWidget(parent)
{
	clsJJSlit_ = clsJJSlit;

	// vertical motors
	CLSJJSlitBladesControl * verticalBladesControl = clsJJSlit_->verticalBladesControl();
	verticalGapEditor_ = new AMExtendedControlEditor(verticalBladesControl->gapPVControl(), verticalBladesControl->statusPVControl());
	verticalCenterEditor_ = new AMExtendedControlEditor(verticalBladesControl->centerPVControl(), verticalBladesControl->statusPVControl());

	QVBoxLayout *editorLayout = new QVBoxLayout;
	editorLayout->addWidget(verticalGapEditor_);
	editorLayout->addWidget(verticalCenterEditor_);

	QLabel *motorLabel = new QLabel("Vertical");
	QHBoxLayout *verticalMotorsLayout = new QHBoxLayout;
	verticalMotorsLayout->addWidget(motorLabel);
	verticalMotorsLayout->addLayout(editorLayout);

	// horizontal motors
	CLSJJSlitBladesControl * horizontalBladesControl = clsJJSlit_->horizontalBladesControl();
	horizontalGapEditor_ = new AMExtendedControlEditor(horizontalBladesControl->gapPVControl(), horizontalBladesControl->statusPVControl());
	horizontalCenterEditor_ = new AMExtendedControlEditor(horizontalBladesControl->centerPVControl(), horizontalBladesControl->statusPVControl());

	editorLayout = new QVBoxLayout;
	editorLayout->addWidget(horizontalGapEditor_);
	editorLayout->addWidget(horizontalCenterEditor_);

	motorLabel = new QLabel("Horizontal");
	QHBoxLayout *horizontalMotorsLayout = new QHBoxLayout;
	horizontalMotorsLayout->addWidget(motorLabel);
	horizontalMotorsLayout->addLayout(editorLayout);

	QVBoxLayout *contentLayout = new QVBoxLayout;
	contentLayout->addLayout(verticalMotorsLayout);
	contentLayout->addLayout(horizontalMotorsLayout);

	setLayout(contentLayout);
}
