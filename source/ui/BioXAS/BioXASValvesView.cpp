#include "BioXASValvesView.h"

BioXASValvesView::BioXASValvesView(BioXASValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Create UI elements.

	allValves_ = new AMExtendedControlEditor(0);
	allValves_->setNoUnitsBox(true);
	allValves_->setTitle("Valves");

	// Create front end UI elements and layouts.

	frontEndValves_ = new AMExtendedControlEditor(0);
	frontEndValves_->setTitle("Front-end valves");
	frontEndValves_->setNoUnitsBox(true);

	frontEnd1_ = new AMExtendedControlEditor(0);
	frontEnd1_->setNoUnitsBox(true);

	frontEnd2_ = new AMExtendedControlEditor(0);
	frontEnd2_->setNoUnitsBox(true);

	frontEnd3_ = new AMExtendedControlEditor(0);
	frontEnd3_->setNoUnitsBox(true);

	frontEnd4_ = new AMExtendedControlEditor(0);
	frontEnd4_->setNoUnitsBox(true);

	frontEnd5_ = new AMExtendedControlEditor(0);
	frontEnd5_->setNoUnitsBox(true);

	frontEnd6_ = new AMExtendedControlEditor(0);
	frontEnd6_->setNoUnitsBox(true);

	QVBoxLayout *frontEndValvesLayout = new QVBoxLayout();
	frontEndValvesLayout->addWidget(frontEnd1_);
	frontEndValvesLayout->addWidget(frontEnd2_);
	frontEndValvesLayout->addWidget(frontEnd3_);
	frontEndValvesLayout->addWidget(frontEnd4_);
	frontEndValvesLayout->addWidget(frontEnd5_);
	frontEndValvesLayout->addWidget(frontEnd6_);

	QGroupBox *frontEndValvesBox = new QGroupBox();
	frontEndValvesBox->setFlat(true);
	frontEndValvesBox->setLayout(frontEndValvesLayout);

	QVBoxLayout *frontEndLayout = new QVBoxLayout();
	frontEndLayout->addWidget(frontEndValves_);
	frontEndLayout->addWidget(frontEndValvesBox);
	frontEndLayout->addStretch();

	// Create Side UI elements and layouts.

	sideValves_ = new AMExtendedControlEditor(0);
	sideValves_->setTitle("Side valves");
	sideValves_->setNoUnitsBox(true);

	side1_ = new AMExtendedControlEditor(0);
	side1_->setNoUnitsBox(true);

	side2_ = new AMExtendedControlEditor(0);
	side2_->setNoUnitsBox(true);

	side3_ = new AMExtendedControlEditor(0);
	side3_->setNoUnitsBox(true);

	side4_ = new AMExtendedControlEditor(0);
	side4_->setNoUnitsBox(true);

	side5_ = new AMExtendedControlEditor(0);
	side5_->setNoUnitsBox(true);

	QVBoxLayout *sideValvesLayout = new QVBoxLayout();
	sideValvesLayout->addWidget(side1_);
	sideValvesLayout->addWidget(side2_);
	sideValvesLayout->addWidget(side3_);
	sideValvesLayout->addWidget(side4_);
	sideValvesLayout->addWidget(side5_);

	QGroupBox *sideValvesBox = new QGroupBox();
	sideValvesBox->setFlat(true);
	sideValvesBox->setLayout(sideValvesLayout);

	QVBoxLayout *sideLayout = new QVBoxLayout();
	sideLayout->addWidget(sideValves_);
	sideLayout->addWidget(sideValvesBox);
	sideLayout->addStretch();

	// Create Main UI elements and layouts.

	mainValves_ = new AMExtendedControlEditor(0);
	mainValves_->setTitle("Main valves");
	mainValves_->setNoUnitsBox(true);

	main1_ = new AMExtendedControlEditor(0);
	main1_->setNoUnitsBox(true);

	main2_ = new AMExtendedControlEditor(0);
	main2_->setNoUnitsBox(true);

	main3_ = new AMExtendedControlEditor(0);
	main3_->setNoUnitsBox(true);

	main4_ = new AMExtendedControlEditor(0);
	main4_->setNoUnitsBox(true);

	main5_ = new AMExtendedControlEditor(0);
	main5_->setNoUnitsBox(true);

	QVBoxLayout *mainValvesLayout = new QVBoxLayout();
	mainValvesLayout->addWidget(main1_);
	mainValvesLayout->addWidget(main2_);
	mainValvesLayout->addWidget(main3_);
	mainValvesLayout->addWidget(main4_);
	mainValvesLayout->addWidget(main5_);

	QGroupBox *mainValvesBox = new QGroupBox();
	mainValvesBox->setFlat(true);
	mainValvesBox->setLayout(mainValvesLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(mainValves_);
	mainLayout->addWidget(mainValvesBox);
	mainLayout->addStretch();

	// Create Imaging UI elements and layouts.

	imagingValves_ = new AMExtendedControlEditor(0);
	imagingValves_->setTitle("Imaging valves");
	imagingValves_->setNoUnitsBox(true);

	imaging1_ = new AMExtendedControlEditor(0);
	imaging1_->setNoUnitsBox(true);

	imaging2_ = new AMExtendedControlEditor(0);
	imaging2_->setNoUnitsBox(true);

	imaging3_ = new AMExtendedControlEditor(0);
	imaging3_->setNoUnitsBox(true);

	imaging4_ = new AMExtendedControlEditor(0);
	imaging4_->setNoUnitsBox(true);

	imaging5_ = new AMExtendedControlEditor(0);
	imaging5_->setNoUnitsBox(true);

	imaging6_ = new AMExtendedControlEditor(0);
	imaging6_->setNoUnitsBox(true);

	imaging7_ = new AMExtendedControlEditor(0);
	imaging7_->setNoUnitsBox(true);


	QVBoxLayout *imagingValvesLayout = new QVBoxLayout();
	imagingValvesLayout->addWidget(imaging1_);
	imagingValvesLayout->addWidget(imaging2_);
	imagingValvesLayout->addWidget(imaging3_);
	imagingValvesLayout->addWidget(imaging4_);
	imagingValvesLayout->addWidget(imaging5_);
	imagingValvesLayout->addWidget(imaging6_);
	imagingValvesLayout->addWidget(imaging7_);

	QGroupBox *imagingValvesBox = new QGroupBox();
	imagingValvesBox->setFlat(true);
	imagingValvesBox->setLayout(imagingValvesLayout);

	QVBoxLayout *imagingLayout = new QVBoxLayout();
	imagingLayout->addWidget(imagingValves_);
	imagingLayout->addWidget(imagingValvesBox);
	imagingLayout->addStretch();

	// Create and set widget layout.

	QHBoxLayout *beamlineValvesLayout = new QHBoxLayout();
	beamlineValvesLayout->addLayout(frontEndLayout);
	beamlineValvesLayout->addLayout(sideLayout);
	beamlineValvesLayout->addLayout(mainLayout);
	beamlineValvesLayout->addLayout(imagingLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(allValves_);
	layout->addLayout(beamlineValvesLayout);

	setLayout(layout);

	// Current settings.

	setValves(valves);

	refresh();
}

BioXASValvesView::~BioXASValvesView()
{

}

void BioXASValvesView::clear()
{
	allValves_->setControl(0);

	frontEndValves_->setControl(0);
	frontEnd1_->setControl(0);
	frontEnd2_->setControl(0);
	frontEnd3_->setControl(0);
	frontEnd4_->setControl(0);
	frontEnd5_->setControl(0);
	frontEnd6_->setControl(0);

	sideValves_->setControl(0);
	side1_->setControl(0);
	side2_->setControl(0);
	side3_->setControl(0);
	side4_->setControl(0);
	side5_->setControl(0);

	mainValves_->setControl(0);
	main1_->setControl(0);
	main2_->setControl(0);
	main3_->setControl(0);
	main4_->setControl(0);
	main5_->setControl(0);

	imagingValves_->setControl(0);
	imaging1_->setControl(0);
	imaging2_->setControl(0);
	imaging3_->setControl(0);
	imaging4_->setControl(0);
	imaging5_->setControl(0);
	imaging6_->setControl(0);
	imaging7_->setControl(0);
}

void BioXASValvesView::update()
{
	if (valves_) {
		allValves_->setControl(valves_);

		frontEndValves_->setControl(valves_->frontEndValves());
		if (valves_->frontEndValves()) {
			frontEnd1_->setControl(valves_->frontEndValves()->vvr1());
			frontEnd2_->setControl(valves_->frontEndValves()->vvr2());
			frontEnd3_->setControl(valves_->frontEndValves()->vvr3());
			frontEnd4_->setControl(valves_->frontEndValves()->vvr4());
			frontEnd5_->setControl(valves_->frontEndValves()->vvr5());
			frontEnd6_->setControl(valves_->frontEndValves()->vvr6());
		}

		sideValves_->setControl(valves_->sideValves());
		if (valves_->sideValves()) {
			side1_->setControl(valves_->sideValves()->vvrSide1());
			side2_->setControl(valves_->sideValves()->vvrSide2());
			side3_->setControl(valves_->sideValves()->vvrSide3());
			side4_->setControl(valves_->sideValves()->vvrSide4());
			side5_->setControl(valves_->sideValves()->vvrSide5());
		}

		mainValves_->setControl(valves_->mainValves());
		if (valves_->mainValves()) {
			main1_->setControl(valves_->mainValves()->vvrMain1());
			main2_->setControl(valves_->mainValves()->vvrMain2());
			main3_->setControl(valves_->mainValves()->vvrMain3());
			main4_->setControl(valves_->mainValves()->vvrMain4());
			main5_->setControl(valves_->mainValves()->vvrMain5());
		}

		imagingValves_->setControl(valves_->imagingValves());
		if (valves_->imagingValves()) {
			imaging1_->setControl(valves_->imagingValves()->vvrImaging1());
			imaging2_->setControl(valves_->imagingValves()->vvrImaging2());
			imaging3_->setControl(valves_->imagingValves()->vvrImaging3());
			imaging4_->setControl(valves_->imagingValves()->vvrImaging4());
			imaging5_->setControl(valves_->imagingValves()->vvrImaging5());
			imaging6_->setControl(valves_->imagingValves()->vvrImaging6());
			imaging7_->setControl(valves_->imagingValves()->vvrImaging7());
		}
	}
}

void BioXASValvesView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASValvesView::setValves(BioXASValves *newValves)
{
	if (valves_ != newValves) {
		valves_ = newValves;
		refresh();

		emit valvesChanged(valves_);
	}
}
