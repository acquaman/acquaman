#include "PGMPersistentView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QMenu>
#include <QAction>


#include "ui/CLS/CLSBeamlineStatusView.h"
//#include "ui/CLS/CLSBeamlineStatusButtonBar.h"


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
	QWidget *beamlineStatusView = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), true);
	connect(beamlineStatusView, SIGNAL(selectedComponentChanged(AMControl*)), this, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)) );

	persistentLayout->addWidget(beamlineStatusView);

	// add stretch for display purpose
	persistentLayout->addStretch();

	return persistentLayout;
}
