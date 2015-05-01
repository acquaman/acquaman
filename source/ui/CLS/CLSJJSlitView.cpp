#include "CLSJJSlitView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/CLS/CLSJJSlit.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"

CLSJJSlitView::CLSJJSlitView(CLSJJSlit *clsJJSlit, QWidget *parent) :
    QWidget(parent)
{
	clsJJSlit_ = clsJJSlit;

//	CLSJJSlitBladesControl * verticalBladesControl = clsJJSlit_->verticalBladesControl();

	verticalGapEditor_ = new AMExtendedControlEditor(clsJJSlit_->verticalGapControl());
	verticalGapEditor_->setControlFormat('f', 3);
	verticalGapEditor_->hideBorder();

	verticalCenterEditor_ = new AMExtendedControlEditor(clsJJSlit_->verticalCenterControl());
	verticalCenterEditor_->setControlFormat('f', 3);
	verticalCenterEditor_->hideBorder();

//	verticalStatusLabel_ = new QLabel("NOT CONNECTED");

//	CLSJJSlitBladesControl * horizontalBladesControl = clsJJSlit_->horizontalBladesControl();

	horizontalGapEditor_ = new AMExtendedControlEditor(clsJJSlit_->horizontalGapControl());
	horizontalGapEditor_->setControlFormat('f', 3);
	horizontalGapEditor_->hideBorder();

	horizontalCenterEditor_ = new AMExtendedControlEditor(clsJJSlit_->horizontalCenterControl());
	horizontalCenterEditor_->setControlFormat('f', 3);
	horizontalCenterEditor_->hideBorder();

//	horizontalStatusLabel_ = new QLabel("NOT CONNECTED");


	QGridLayout *contentGridLayout = new QGridLayout;
	contentGridLayout->addWidget(new QLabel("Direction"),  0, 0, 1, 1);
	contentGridLayout->addWidget(new QLabel("Gap"),        0, 1, 1, 1);
	contentGridLayout->addWidget(new QLabel("Center"),     0, 2, 1, 1);
	contentGridLayout->addWidget(new QLabel("Status"),     0, 3, 1, 1);

	contentGridLayout->addWidget(new QLabel("Vertical"),   1, 0, 1, 1);
	contentGridLayout->addWidget(verticalGapEditor_,       1, 1, 1, 1);
	contentGridLayout->addWidget(verticalCenterEditor_,    1, 2, 1, 1);
//	contentGridLayout->addWidget(verticalStatusLabel_,     1, 3, 1, 1);

	contentGridLayout->addWidget(new QLabel("Horizontal"), 2, 0, 1, 1);
	contentGridLayout->addWidget(horizontalGapEditor_,     2, 1, 1, 1);
	contentGridLayout->addWidget(horizontalCenterEditor_,  2, 2, 1, 1);
//	contentGridLayout->addWidget(horizontalStatusLabel_,   2, 3, 1, 1);

	setLayout(contentGridLayout);

	connect(clsJJSlit_, SIGNAL(connected(bool)), this, SLOT(onJJSlitConnected()));
//	connect(clsJJSlit_, SIGNAL(verticalStatusStringChanged(QString)), verticalStatusLabel_, SLOT(setText(QString)));
//	connect(clsJJSlit_, SIGNAL(horizontalStatusStringChanged(QString)), horizontalStatusLabel_, SLOT(setText(QString)));

	onJJSlitConnected();
}

CLSJJSlitView::~CLSJJSlitView()
{

}

void CLSJJSlitView::onJJSlitConnected()
{
//	if (clsJJSlit_->isConnected()) {
//		CLSJJSlitBladesControl * verticalBladesControl = clsJJSlit_->verticalBladesControl();
//		verticalStatusLabel_->setText(verticalBladesControl->statusString());

//		CLSJJSlitBladesControl * horizontalBladesControl = clsJJSlit_->horizontalBladesControl();
//		horizontalStatusLabel_->setText(horizontalBladesControl->statusString());
//	}
}
