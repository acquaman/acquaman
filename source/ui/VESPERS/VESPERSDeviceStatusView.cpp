#include "VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSTemperatureView.h"

#include <QStackedWidget>
#include <QButtonGroup>
#include <QVBoxLayout>

VESPERSDeviceStatusView::VESPERSDeviceStatusView(QWidget *parent) :
	QWidget(parent)
{
	VESPERSTemperatureView *temperature = new VESPERSTemperatureView;
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(temperature, 0, Qt::AlignCenter);
	setLayout(layout);
}
