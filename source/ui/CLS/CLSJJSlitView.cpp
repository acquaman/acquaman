#include "CLSJJSlitView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>

#include "beamline/CLS/CLSJJSlit.h"
#include "ui/beamline/AMExtendedControlEditor.h"

CLSJJSlitView::CLSJJSlitView(CLSJJSlit *clsJJSlit, QWidget *parent) :
    QWidget(parent)
{
	clsJJSlit_ = clsJJSlit;

	verticalGapSpinBox_ = createDoubleSpinBox();
	verticalCenterSpinBox_ = createDoubleSpinBox();
	verticalStatusLabel_ = new QLabel("NOT CONNECTED");

	horizontalGapSpinBox_ = createDoubleSpinBox();
	horizontalCenterSpinBox_ = createDoubleSpinBox();
	horizontalStatusLabel_ = new QLabel("NOT CONNECTED");


	QGridLayout *contentGridLayout = new QGridLayout;

	contentGridLayout->addWidget(new QLabel("Direction"),  0, 0, 1, 1);
	contentGridLayout->addWidget(new QLabel("Gap"),        0, 1, 1, 1);
	contentGridLayout->addWidget(new QLabel("Center"),     0, 2, 1, 1);
	contentGridLayout->addWidget(new QLabel("Status"),     0, 3, 1, 1);

	contentGridLayout->addWidget(new QLabel("Vertical"),   1, 0, 1, 1);
	contentGridLayout->addWidget(verticalGapSpinBox_,      1, 1, 1, 1);
	contentGridLayout->addWidget(verticalCenterSpinBox_,   1, 2, 1, 1);
	contentGridLayout->addWidget(verticalStatusLabel_,     1, 3, 1, 1);

	contentGridLayout->addWidget(new QLabel("Horizontal"), 2, 0, 1, 1);
	contentGridLayout->addWidget(horizontalGapSpinBox_,    2, 1, 1, 1);
	contentGridLayout->addWidget(horizontalCenterSpinBox_, 2, 2, 1, 1);
	contentGridLayout->addWidget(horizontalStatusLabel_,   2, 3, 1, 1);

	setLayout(contentGridLayout);

	connect(clsJJSlit_, SIGNAL(connected(bool)), this, SLOT(onJJSlitConnected()));
	connect(clsJJSlit_, SIGNAL(verticalGapValueChanged(double)), verticalGapSpinBox_, SLOT(setValue(double)));
	connect(clsJJSlit_, SIGNAL(verticalCenterValueChanged(double)), verticalCenterSpinBox_, SLOT(setValue(double)));
	connect(clsJJSlit_, SIGNAL(verticalStatusStringChanged(QString)), verticalStatusLabel_, SLOT(setText(QString)));
	connect(clsJJSlit_, SIGNAL(horizontalGapValueChanged(double)), horizontalGapSpinBox_, SLOT(setValue(double)));
	connect(clsJJSlit_, SIGNAL(horizontalCenterValueChanged(double)), horizontalCenterSpinBox_, SLOT(setValue(double)));
	connect(clsJJSlit_, SIGNAL(horizontalStatusStringChanged(QString)), horizontalStatusLabel_, SLOT(setText(QString)));

	connect(verticalGapSpinBox_, SIGNAL(editingFinished()), this, SLOT(onVerticalGapSpinBoxEditFinished()));
	connect(verticalCenterSpinBox_, SIGNAL(editingFinished()), this, SLOT(onVerticalCenterSpinBoxEditFinished()));
	connect(horizontalGapSpinBox_, SIGNAL(editingFinished()), this, SLOT(onHorizontalGapSpinBoxEditFinished()));
	connect(horizontalCenterSpinBox_, SIGNAL(editingFinished()), this, SLOT(onHorizontalCenterSpinBoxEditFinished()));

	onJJSlitConnected();
}

void CLSJJSlitView::onJJSlitConnected()
{
	if (clsJJSlit_->isConnected()) {
		CLSJJSlitBladesControl * verticalBladesControl = clsJJSlit_->verticalBladesControl();
		verticalGapSpinBox_->setValue(verticalBladesControl->gap());
		verticalCenterSpinBox_->setValue(verticalBladesControl->center());
		verticalStatusLabel_->setText(verticalBladesControl->statusString());

		CLSJJSlitBladesControl * horizontalBladesControl = clsJJSlit_->horizontalBladesControl();
		horizontalGapSpinBox_->setValue(horizontalBladesControl->gap());
		horizontalCenterSpinBox_->setValue(horizontalBladesControl->center());
		horizontalStatusLabel_->setText(horizontalBladesControl->statusString());
	}
}

void CLSJJSlitView::onVerticalGapSpinBoxEditFinished()
{
	clsJJSlit_->moveVerticalGap(verticalGapSpinBox_->value());
}

void CLSJJSlitView::onVerticalCenterSpinBoxEditFinished()
{
	clsJJSlit_->moveVerticalCenter(verticalCenterSpinBox_->value());
}

void CLSJJSlitView::onHorizontalGapSpinBoxEditFinished()
{
	clsJJSlit_->moveHorizontalGap(horizontalGapSpinBox_->value());
}

void CLSJJSlitView::onHorizontalCenterSpinBoxEditFinished()
{
	clsJJSlit_->moveHorizontalCenter(horizontalCenterSpinBox_->value());
}

QDoubleSpinBox * CLSJJSlitView::createDoubleSpinBox()
{
	QDoubleSpinBox *spinBox = new QDoubleSpinBox;
	spinBox->setMinimumWidth(150);
	spinBox->setAlignment(Qt::AlignCenter);
	spinBox->setSuffix(" mm");
	spinBox->setDecimals(3);
	spinBox->setSingleStep(0.001);
	spinBox->setRange(-10, 10);

	return spinBox;
}

