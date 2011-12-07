#include "REIXSSampleChamberButtonPanel.h"
#include "ui_REIXSSampleChamberButtonPanel.h"

#include "ui/beamline/AMControlMoveButton.h"
#include "beamline/REIXS/REIXSBeamline.h"

REIXSSampleChamberButtonPanel::REIXSSampleChamberButtonPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::REIXSSampleChamberButtonPanel)
{
    ui->setupUi(this);

	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	ui->sampleXup->setControl(chamber->x());
	ui->sampleXup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleXup->setStepSizeIndex(1);
	ui->sampleXdown->setControl(chamber->x());
	ui->sampleXdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleXdown->setStepSizeIndex(1);
	ui->sampleXdown->setDirectionReversed(true);

	ui->sampleYup->setControl(chamber->y());
	ui->sampleYup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleYup->setStepSizeIndex(1);
	ui->sampleYdown->setControl(chamber->y());
	ui->sampleYdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	ui->sampleYdown->setStepSizeIndex(1);
	ui->sampleYdown->setDirectionReversed(true);

	ui->sampleZup->setControl(chamber->z());
	ui->sampleZup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->sampleZup->setStepSizeIndex(2);
	ui->sampleZup->setDirectionReversed(true);// yup; down is up. Go figure.
	ui->sampleZdown->setControl(chamber->z());
	ui->sampleZdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->sampleZdown->setStepSizeIndex(2);

	ui->sampleCW->setControl(chamber->r());
	ui->sampleCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->sampleCW->setStepSizeIndex(1);
	ui->sampleCCW->setControl(chamber->r());
	ui->sampleCCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->sampleCCW->setDirectionReversed(true);
	ui->sampleCCW->setStepSizeIndex(1);

	ui->loadLockCW->setControl(chamber->loadLockR());
	ui->loadLockCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->loadLockCW->setStepSizeIndex(1);
	ui->loadLockCCW->setControl(chamber->loadLockR());
	ui->loadLockCCW->setStepSizes(QList<double>() << 1 << 5 << 10 << 45 << 90);
	ui->loadLockCCW->setDirectionReversed(true);
	ui->loadLockCCW->setStepSizeIndex(1);

	ui->loadLockZup->setControl(chamber->loadLockZ());
	ui->loadLockZup->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->loadLockZup->setStepSizeIndex(2);
	ui->loadLockZdown->setControl(chamber->loadLockZ());
	ui->loadLockZdown->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10 << 50 << 100);
	ui->loadLockZdown->setDirectionReversed(true);
	ui->loadLockZdown->setStepSizeIndex(2);

	connect(ui->stopAll, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
}

REIXSSampleChamberButtonPanel::~REIXSSampleChamberButtonPanel()
{
	delete ui;
}

void REIXSSampleChamberButtonPanel::onStopButtonClicked()
{
	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	chamber->x()->stop();
	chamber->y()->stop();
	chamber->z()->stop();
	chamber->r()->stop();
	chamber->loadLockR()->stop();
	chamber->loadLockZ()->stop();
}
