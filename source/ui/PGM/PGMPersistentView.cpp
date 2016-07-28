#include "PGMPersistentView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>

#include "beamline/PGM/PGMBeamline.h"

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/PGM/PGMBladeCurrentView.h"
#include "ui/PGM/PGMBeamStatusView.h"
#include "ui/PGM/PGMBPMStatusView.h"


#include "util/AMHDF5Driver.h"
#include "dataman/HDF5/AMHDF5File.h"
#include "dataman/HDF5/AMHDF5Group.h"

PGMPersistentView::PGMPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// setup the Beamline persistent view component
	QLayout * persistentViewLayout = createPersistentLayout();

	QGroupBox *persistentViewGroupBox = new QGroupBox("VLS-PGM Beamline");
	persistentViewGroupBox->setLayout(persistentViewLayout);

	QVBoxLayout *mainViewLayout = new QVBoxLayout;
	mainViewLayout->addWidget(persistentViewGroupBox);

	AMHDF5File *file = new AMHDF5File("/home/butlerm/beamline/programming/hdf5-data/test.h5");
//	AMHDF5File *file = new AMHDF5File("/Users/hunterd/beamline/programming/hdf5-data/test.h5");
	file->createHDF5File(AMHDF5File::OverwriteExisting);
	file->addGroup("supa-group");
	file->closeHDF5File();
	file->deleteLater();

//	AMHDF5Group *group = new AMHDF5Group("TestGroup");
//	group->createHDF5Group(0);
//	group->openHDF5Group(0);
//	group->isOpen();
//	group->closeHDF5Group();

	QVector<double> data = QVector<double>(5);
	AMHDF5Driver test;
	hid_t fileId = test.createFile();
//	hid_t fileId = test.openFile();
	hid_t spaceId = test.addDataSpace();
	hid_t dataSetId = test.addDataSet(fileId, spaceId);
//	hid_t dataSetId = test.openDataSet(fileId);
//	qDebug() << "Read: " << test.readDataSet(dataSetId, data.data());
//	qDebug() << data;
	data.fill(24);
	qDebug() << "Write: " << test.writeDataSet(dataSetId, data.data());
//	qDebug() << "Read: " << test.readDataSet(dataSetId, data.data());
//	qDebug() << data;
	hid_t attributeId = test.addAttribute(dataSetId, test.getDataSpace(dataSetId));
	test.closeAttribute(attributeId);
	test.closeDataSet(dataSetId);
	test.closeDataSpace(spaceId);
	test.closeFile(fileId);

	setLayout(mainViewLayout);

	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	setMaximumHeight(1000);
	setMinimumHeight(800);
	setMaximumWidth(400);
	setMinimumWidth(400);
}

QLayout* PGMPersistentView::createPersistentLayout()
{
	// create the beamline status view
	QWidget *beamlineStatusView = new CLSBeamlineStatusView(PGMBeamline::pgm()->beamlineStatus(), true);
	connect(beamlineStatusView, SIGNAL(selectedComponentChanged(AMControl*)), this, SIGNAL(beamlineStatusSelectedComponentChanged(AMControl*)) );

// NOTES:  remove this from the persistent view for this moment. We will decide what we want later..
//	// create the PGM blade current view
//	PGMBladeCurrentView *bladeCurrentView = new PGMBladeCurrentView;

//    QHBoxLayout *bladeLayout = new QHBoxLayout;
//    bladeLayout->addWidget(bladeCurrentView);
//    QGroupBox *bladeCurrentBox = new QGroupBox("Blade Currents");
//    bladeCurrentBox->setLayout(bladeLayout);

	beamStatusView_ = new PGMBeamStatusView();
	bpmStatusView_ = new PGMBPMStatusView();

	// For organizational purpose on the persistent view. Create a groupbox with a header and then put a BPM layout inside of it.
	QGroupBox *bpmBox = new QGroupBox("Beam Position Monitors");
	QHBoxLayout *bpmLayout = new QHBoxLayout;
	bpmLayout->addWidget(bpmStatusView_);
	bpmBox->setLayout(bpmLayout);


	// Note: Lucia perfers her beamline to be referreed to by it's full acroynm. It's not nessisary to use
	// the full acroynm in the code but whenever it's displayed to user please try to use VLS-PGM.

	energyControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->energy());
	energyControlEditor_->setControlFormat('f', 3);
	energyControlEditor_->setUnits("eV");

	gratingControlEditor_ = new AMExtendedControlEditor(PGMBeamline::pgm()->gratingSelectionControl());

	// Main layout
	QVBoxLayout *mainPanelLayout = new QVBoxLayout;
	mainPanelLayout->addWidget(beamlineStatusView);
	mainPanelLayout->addWidget(beamStatusView_);
	mainPanelLayout->addWidget(bpmBox);
	mainPanelLayout->addWidget(energyControlEditor_);
	mainPanelLayout->addWidget(gratingControlEditor_);
//    mainPanelLayout->addWidget(bladeCurrentBox);

	// Add final stretch to the layout, so the widgets appear new the top of the view.
	mainPanelLayout->addStretch();

	return mainPanelLayout;
}
