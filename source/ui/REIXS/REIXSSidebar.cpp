#include "REIXSSidebar.h"
#include "ui_REIXSSidebar.h"

#include <QBoxLayout>

#include "ui/REIXS/REIXSXESSpectrometerControlEditor.h"
#include "beamline/REIXS/REIXSBeamline.h"
#include "ui/REIXS/REIXSActionBasedControlEditor.h"

REIXSSidebar::REIXSSidebar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::REIXSSidebar)
{
	ui->setupUi(this);
	ui->verticalLayout->insertWidget(1, new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer()));

	beamlineEnergyEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->energy());
	ui->beamlineFormLayout->setWidget(1, QFormLayout::FieldRole, beamlineEnergyEditor_);

	monoSlitEditor_ = new REIXSActionBasedControlEditor(REIXSBeamline::bl()->photonSource()->monoSlit());
	ui->beamlineFormLayout->setWidget(4, QFormLayout::FieldRole, monoSlitEditor_);
}

REIXSSidebar::~REIXSSidebar()
{
    delete ui;
}
