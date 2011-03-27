#include "VESPERSBeamlineView.h"
#include "ui/AMControlEditor.h"
#include "ui/VESPERS/VESPERSEndstationView.h"
#include <QHBoxLayout>

VESPERSBeamlineView::VESPERSBeamlineView(QWidget *parent) :
	QWidget(parent)
{
	VESPERSEndstationView *endstationView = new VESPERSEndstationView(this);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(endstationView);
	setLayout(layout);

	this->setMinimumSize(1000, 465);
}
