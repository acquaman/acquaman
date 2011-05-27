#include "VESPERSTemperatureView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

#include <QGridLayout>
#include <QLabel>

VESPERSTemperatureView::VESPERSTemperatureView(QWidget *parent) :
	QWidget(parent)
{
	AMControlSet *set = VESPERSBeamline::vespers()->temperatureSet();
	QLabel *name;
	AMReadOnlyPVwStatusControl *current;

	QGridLayout *layout = new QGridLayout;
	layout->setSpacing(8);

	for (int i = 0; i < set->count(); i++){

		current = qobject_cast<AMReadOnlyPVwStatusControl *>(set->at(i));

		if (current){

			name = new QLabel(current->name().remove("Temperature "));
			layout->addWidget(name, i, 0, 1, 2);
		}
	}

	setLayout(layout);
}
