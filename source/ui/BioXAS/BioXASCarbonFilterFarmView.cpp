#include "BioXASCarbonFilterFarmView.h"
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASCarbonFilterFarmView::BioXASCarbonFilterFarmView(BioXASCarbonFilterFarm *filterFarm, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	filterFarm_ = 0;

	// Create UI elements.

	upstreamPositionEditor_ = new AMExtendedControlEditor(0);
	upstreamPositionEditor_->setTitle("Position");

	upstreamStatusEditor_ = new AMExtendedControlEditor(0, 0, true);
	upstreamStatusEditor_->setTitle("Status");

	upstreamWindowEditor_ = new AMExtendedControlEditor(0);
	upstreamWindowEditor_->setTitle("Active window");

	upstreamThicknessEditor_ = new AMExtendedControlEditor(0);
	upstreamThicknessEditor_->setTitle("Filter thickness");

	downstreamPositionEditor_ = new AMExtendedControlEditor(0);
	downstreamPositionEditor_->setTitle("Position");

	downstreamStatusEditor_ = new AMExtendedControlEditor(0, 0, true);
	downstreamStatusEditor_->setTitle("Status");

	downstreamWindowEditor_ = new AMExtendedControlEditor(0);
	downstreamWindowEditor_->setTitle("Active window");

	downstreamThicknessEditor_ = new AMExtendedControlEditor(0);
	downstreamThicknessEditor_->setTitle("Filter thickness");

	totalThicknessEditor_ = new AMExtendedControlEditor(0);
	totalThicknessEditor_->setTitle("Total filter thickness");

	// Create and set layouts.

	QWidget *upstreamActuatorView = createActuatorView("Upstream actuator", upstreamPositionEditor_, upstreamStatusEditor_, upstreamWindowEditor_, upstreamThicknessEditor_);

	QWidget *downstreamActuatorView = createActuatorView("Downstream actuator", downstreamPositionEditor_, downstreamStatusEditor_, downstreamWindowEditor_, downstreamThicknessEditor_);

	QHBoxLayout *actuatorsLayout = new QHBoxLayout();
	actuatorsLayout->addWidget(upstreamActuatorView);
	actuatorsLayout->addWidget(downstreamActuatorView);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(actuatorsLayout);
	layout->addWidget(totalThicknessEditor_);

	setLayout(layout);

	// Current settings.

	setFilterFarm(filterFarm);
}

BioXASCarbonFilterFarmView::~BioXASCarbonFilterFarmView()
{

}

void BioXASCarbonFilterFarmView::setFilterFarm(BioXASCarbonFilterFarm *newFilterFarm)
{
	if (filterFarm_ != newFilterFarm) {

		if (filterFarm_) {

			// Clear UI elements.

			upstreamWindowEditor_->setControl(0);
			upstreamThicknessEditor_->setControl(0);
			downstreamWindowEditor_->setControl(0);
			downstreamThicknessEditor_->setControl(0);
			totalThicknessEditor_->setControl(0);

			upstreamPositionEditor_->setControl(0);
			upstreamStatusEditor_->setControl(0);
			downstreamPositionEditor_->setControl(0);
			downstreamStatusEditor_->setControl(0);
		}

		filterFarm_ = newFilterFarm;

		if (filterFarm_) {

			// Update UI elements.

			upstreamWindowEditor_->setControl(filterFarm_->upstreamActuatorWindowControl());
			upstreamThicknessEditor_->setControl(filterFarm_->upstreamActuatorFilterThicknessControl());
			downstreamWindowEditor_->setControl(filterFarm_->downstreamActuatorWindowControl());
			downstreamThicknessEditor_->setControl(filterFarm_->downstreamActuatorFilterThicknessControl());
			totalThicknessEditor_->setControl(filterFarm_->filterThicknessControl());

			upstreamPositionEditor_->setControl(filterFarm_->upstreamPositionControl());
			upstreamStatusEditor_->setControl(filterFarm_->upstreamStatusControl());
			downstreamPositionEditor_->setControl(filterFarm_->downstreamPositionControl());
			downstreamStatusEditor_->setControl(filterFarm_->downstreamStatusControl());
		}

		emit filterFarmChanged(filterFarm_);
	}
}

QWidget* BioXASCarbonFilterFarmView::createActuatorView(const QString &title, QWidget *positionEditor, QWidget *statusEditor, QWidget *windowEditor, QWidget *thicknessEditor)
{
	QLayout *viewLayout = createActuatorViewLayout(positionEditor, statusEditor, windowEditor, thicknessEditor);

	QGroupBox *view = new QGroupBox();
	view->setTitle(title);
	view->setLayout(viewLayout);

	return view;
}

QLayout* BioXASCarbonFilterFarmView::createActuatorViewLayout(QWidget *positionEditor, QWidget *statusEditor, QWidget *windowEditor, QWidget *thicknessEditor)
{
	QHBoxLayout *positionLayout = new QHBoxLayout();
	positionLayout->addWidget(positionEditor);
	positionLayout->addWidget(statusEditor);

	QVBoxLayout *actuatorLayout = new QVBoxLayout();
	actuatorLayout->addLayout(positionLayout);
	actuatorLayout->addWidget(windowEditor);
	actuatorLayout->addWidget(thicknessEditor);

	return actuatorLayout;
}
