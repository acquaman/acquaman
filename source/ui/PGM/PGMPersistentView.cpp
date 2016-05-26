#include "PGMPersistentView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>


#include "ui/CLS/CLSBeamlineStatusButtonBar.h"


PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// setup the Beamline persistent view component
	QLayout * persistentViewLayout = createPersistentLayout();

	QGroupBox *persistentViewGroupBox = new QGroupBox("VLS-PGM");
	persistentViewGroupBox->setLayout(persistentViewLayout);

	QVBoxLayout *mainViewLayout = new QVBoxLayout;
	mainViewLayout->addWidget(persistentViewGroupBox);

	setLayout(mainViewLayout);
	setFixedWidth(350);
}

QLayout* PGMPersistentView::createPersistentLayout()
{
	QVBoxLayout *persistentLayout = new QVBoxLayout;

	// create the beamline status view
	QWidget *beamlineStatusWidget = createBeamlineStatusWidget();
	if (beamlineStatusWidget) {
		persistentLayout->addWidget(beamlineStatusWidget);
	}

	// add stretch for display purpose
	persistentLayout->addStretch();

	return persistentLayout;
}

QWidget* PGMPersistentView::createBeamlineStatusWidget()
{
	QWidget *beamlineStatusWidget = 0;

	CLSBeamlineStatus *beamlineStatus = PGMBeamline::pgm()->beamlineStatus();
	if (beamlineStatus) {

		CLSBeamlineStatusButtonBar *beamlineStatusButtons = new CLSBeamlineStatusButtonBar(beamlineStatus);
//		connect( CLSBeamlineStatusButtonBar, SIGNAL(selectedControlChanged(AMControl*)), this, SIGNAL(beamStatusButtonsSelectedControlChanged(AMControl*)) );

		QHBoxLayout *beamStatusBoxLayout = new QHBoxLayout();
		beamStatusBoxLayout->addStretch();
		beamStatusBoxLayout->addWidget(beamlineStatusButtons);
		beamStatusBoxLayout->addStretch();

		beamlineStatusWidget = new QGroupBox("Beamline status");
		beamlineStatusWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		beamlineStatusWidget->setLayout(beamStatusBoxLayout);
	}

	return beamlineStatusWidget;
}
