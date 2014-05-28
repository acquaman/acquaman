#include "VESPERSDatabaseDuplicateEntryPatchView.h"

#include <QPushButton>
#include <QHBoxLayout>

VESPERSDatabaseDuplicateEntryPatchView::VESPERSDatabaseDuplicateEntryPatchView(VESPERSDatabaseDuplicateEntryPatch *patch, QWidget *parent)
	: QWidget(parent)
{
	patch_ = patch;

	QPushButton *startButton = new QPushButton("Start");
	connect(startButton, SIGNAL(clicked()), patch_, SLOT(start()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(startButton);
	setLayout(layout);
}
