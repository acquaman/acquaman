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

	QLabel *name = new QLabel(chamber_->name());

	QToolButton *minus = new QToolButton;
	minus->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minus, SIGNAL(clicked()), chamber_, SLOT(decreaseSensitivity()));
	connect(chamber_, SIGNAL(minimumSensitivity(bool)), minus, SLOT(setDisabled(bool)));

	QToolButton *plus = new QToolButton;
	plus->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plus, SIGNAL(clicked()), chamber_, SLOT(increaseSensitivity()));
	connect(chamber_, SIGNAL(maximumSensitivity(bool)), plus, SLOT(setDisabled(bool)));

	output_ = new QLabel;

	connect(chamber_, SIGNAL(readingsChanged()), this, SLOT(onReadingsChanged()));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	setContextMenuPolicy(Qt::CustomContextMenu);

	QHBoxLayout *ionChamberViewLayout = new QHBoxLayout;
	ionChamberViewLayout->addWidget(name, 0, Qt::AlignRight);
	ionChamberViewLayout->addWidget(minus);
	ionChamberViewLayout->addWidget(plus);
	ionChamberViewLayout->addWidget(output_, 0, Qt::AlignCenter);

	setLayout(ionChamberViewLayout);
}

void AMIonChamberView::onReadingsChanged()
{
	QFont font(this->font());
	QPalette palette(this->palette());

	switch(state_){

	case Counts:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(chamber_->counts()) + " counts");
		break;

	case Voltage:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(chamber_->voltage()) + " V");
		break;

	case Status:

		font.setBold(true);
		font.setPointSize(12);

		if (chamber_->withinLinearRange()){

			palette.setColor(QPalette::WindowText, Qt::darkGreen);
			output_->setText(withinRange_);
		}
		else if (chamber_->voltageTooHigh()){

			palette.setColor(QPalette::WindowText, Qt::red);
			output_->setText(tooHigh_);
		}
		else if (chamber_->voltageTooLow()){

			palette.setColor(QPalette::WindowText, Qt::red);
			output_->setText(tooLow_);
		}
		else{

			palette.setColor(QPalette::WindowText, Qt::blue);
			output_->setText("Invalid");
		}

		break;
	}

	output_->setPalette(palette);
	output_->setFont(font);
}

void AMIonChamberView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Status View");
	if (state_ == Status)
		temp->setDisabled(true);

	temp = popup.addAction("Counts View");
	if (state_ == Counts)
		temp->setDisabled(true);

	temp = popup.addAction("Voltage View");
	if (state_ == Voltage)
		temp->setDisabled(true);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Status View")
			state_ = Status;
		else if (temp->text() == "Counts View")
			state_ = Counts;
		else if (temp->text() == "Voltage View")
			state_ = Voltage;

		onReadingsChanged();
	}
}
