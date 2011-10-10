#include "AMIonChamberView.h"

#include <QToolButton>
#include <QHBoxLayout>

AMIonChamberView::AMIonChamberView(AMIonChamber *chamber, QWidget *parent)
	: QWidget(parent)
{
	chamber_ = chamber;

	QLabel *name = new QLabel(chamber_->description());

	QToolButton *minus = new QToolButton;
	minus->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minus, SIGNAL(clicked()), this, SLOT(onDecreaseSensitivityClicked()));
	connect(chamber_, SIGNAL(minimumSensitivity(bool)), minus, SLOT(setDisabled(bool)));

	QToolButton *plus = new QToolButton;
	plus->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plus, SIGNAL(clicked()), this, SLOT(onIncreaseSensitivityClicked()));
	connect(chamber_, SIGNAL(maximumSensitivity(bool)), plus, SLOT(setDisabled(bool)));

	output_ = new QLabel;

	connect(chamber_, SIGNAL(readingsChanged()), this, SLOT(onReadingsChanged()));

	QHBoxLayout *ionChamberViewLayout = new QHBoxLayout;
	ionChamberViewLayout->addWidget(name);
	ionChamberViewLayout->addWidget(minus);
	ionChamberViewLayout->addWidget(plus);
	ionChamberViewLayout->addWidget(output_);

	setLayout(ionChamberViewLayout);
}

void AMIonChamberView::onReadingsChanged()
{
	// Flesh this out!
}
