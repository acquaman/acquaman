#include "AMMockPersistentView.h"
#include "beamline/AMControlSet.h"
#include "ui/beamline/AMExtendedControlEditor.h"

#include <QVBoxLayout>
#include <QLabel>

AMMockPersistentView::AMMockPersistentView(AMControlSet* controls,
                                           QWidget *parent) :
    QWidget(parent)
{
	setupUi(controls);
}

void AMMockPersistentView::setupUi(AMControlSet* controls)
{
	setFixedWidth(350);
	setContentsMargins(5,5,5,5);

	QVBoxLayout* mainLayout = new QVBoxLayout();


	QLabel* controlsSectionLabel = new QLabel("Controls");
	mainLayout->addWidget(controlsSectionLabel);

	foreach(AMControl* control, controls->toList()) {

		AMExtendedControlEditor* controlEditor = new AMExtendedControlEditor(control);
		mainLayout->addWidget(controlEditor);
	}

	mainLayout->addStretch(0);
	setLayout(mainLayout);
}
