#include "AMIonChamberView.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

AMIonChamberView::AMIonChamberView(AMIonChamber *chamber, QWidget *parent)
	: QWidget(parent)
{
	chamber_ = chamber;
	state_ = Status;
	tooHigh_ = "Too High!";
	tooLow_ = "Too Low!";
	withinRange_ = "Good!";

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
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	QHBoxLayout *ionChamberViewLayout = new QHBoxLayout;
	ionChamberViewLayout->addWidget(name);
	ionChamberViewLayout->addWidget(minus);
	ionChamberViewLayout->addWidget(plus);
	ionChamberViewLayout->addWidget(output_);

	setLayout(ionChamberViewLayout);
}

void AMIonChamberView::onReadingsChanged()
{
	switch(state_){

	case Counts:
		output_->setText(chamber_->counts() + " counts");
		break;

	case Voltage:
		output_->setText(chamber_->voltage() + " V");
		break;

	case Status:
		QFont font(this->font());
		font.setBold(true);
		font.setPointSize(12);

		if (chamber_->withinLinearRange()){

			output_->setPalette(QPalette(Qt::green));
			output_->setText(withinRange_);
		}
		else if (chamber_->voltageTooHigh()){

			output_->setPalette(QPalette(Qt::red));
			output_->setText(tooHigh_);
		}
		else if (chamber_->voltageTooLow()){

			output_->setPalette(QPalette(Qt::red));
			output_->setText(tooLow_);
		}
		else{

			output_->setPalette(QPalette(Qt::blue));
			output_->setText("Invalid");
		}

		break;
	}
}

void AMIonChamberView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup("Switch to...", this);

	QAction *temp = popup.addAction("Status View");
	if (state_ == Status)
		temp->setDisabled(true);

	temp = popup.addAction("Counts View");
	if (state_ == Counts)
		temp->setDisabled(true);

	temp = popup.addAction("Voltage View");
	if (state_ == Voltage)
		temp->setDisabled(true);

	temp = popup.exec(pos);

	// If a valid action was selected.
	if (temp){

		if (temp == "Status View")
			state_ = Status;
		else if (temp == "Counts View")
			state_ = Counts;
		else if (temp == "Voltage View")
			state_ = Voltage;

		onReadingsChanged();
	}
}
