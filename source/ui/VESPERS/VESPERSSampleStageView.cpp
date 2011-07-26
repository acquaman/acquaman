#include "VESPERSSampleStageView.h"
#include "ui/AMStopButton.h"

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSSampleStageView::VESPERSSampleStageView(QWidget *parent) :
	QWidget(parent)
{
	sampleStage_ = VESPERSBeamline::vespers()->pseudoSampleStage();
	connect(sampleStage_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(sampleStage_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
	connect(sampleStage_, SIGNAL(horizontalMoveError(bool)), this, SLOT(onHorizontalMoveError(bool)));
	connect(sampleStage_, SIGNAL(verticalMoveError(bool)), this, SLOT(onVerticalMoveError(bool)));
	connect(sampleStage_, SIGNAL(normalMoveError(bool)), this, SLOT(onNormalMoveError(bool)));

	jog_ = new QDoubleSpinBox;
	jog_->setSuffix(" mm");
	jog_->setSingleStep(0.001);
	jog_->setMaximum(5.0);
	jog_->setMinimum(0.0000);
	jog_->setValue(0.050);
	jog_->setDecimals(3);
	jog_->setAlignment(Qt::AlignCenter);

	horizontal_ = new QLineEdit;
	horizontal_->setAlignment(Qt::AlignCenter);
	connect(horizontal_, SIGNAL(returnPressed()), this, SLOT(onHorizontalSetpoint()));
	connect(sampleStage_, SIGNAL(horizontalSetpointChanged(double)), this, SLOT(onHorizontalChanged(double)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *h = new QLabel("H :");
	h->setFont(font);
	QLabel *v = new QLabel("V :");
	v->setFont(font);

	QGridLayout *hLayout = new QGridLayout;
	hLayout->addWidget(horizontal_, 0, 1, 1, 4, Qt::AlignCenter);
	hLayout->addWidget(h, 0, 0, 1, 1, Qt::AlignCenter);
	hLayout->addWidget(new QLabel("mm"), 0, 4, 1, 1);

	vertical_ = new QLineEdit;
	vertical_->setAlignment(Qt::AlignCenter);
	connect(vertical_, SIGNAL(returnPressed()), this, SLOT(onVerticalSetpoint()));
	connect(sampleStage_, SIGNAL(verticalSetpointChanged(double)), this, SLOT(onVerticalChanged(double)));

	QGridLayout *vLayout = new QGridLayout;
	vLayout->addWidget(vertical_, 0, 1, 1, 4, Qt::AlignCenter);
	vLayout->addWidget(v, 0, 0, 1, 1, Qt::AlignCenter);
	vLayout->addWidget(new QLabel("mm"), 0, 4, 1, 1);

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25));

	goUp_ = new QToolButton;
	goUp_->setIcon(QIcon(":/go-up.png"));
	connect(goUp_, SIGNAL(clicked()), this, SLOT(onUpClicked()));

	goDown_ = new QToolButton;
	goDown_->setIcon(QIcon(":/go-down.png"));
	connect(goDown_, SIGNAL(clicked()), this, SLOT(onDownClicked()));

	goLeft_ = new QToolButton;
	goLeft_->setIcon(QIcon(":/go-previous.png"));
	connect(goLeft_, SIGNAL(clicked()), this, SLOT(onLeftClicked()));

	goRight_ = new QToolButton;
	goRight_->setIcon(QIcon(":/go-next.png"));
	connect(goRight_, SIGNAL(clicked()), this, SLOT(onRightClicked()));

	buttons_ = new QButtonGroup(this);
	buttons_->addButton(goUp_, 0);
	buttons_->addButton(goDown_, 1);
	buttons_->addButton(goLeft_, 2);
	buttons_->addButton(goRight_, 3);

	AMGroupStopButton *stop = new AMGroupStopButton(VESPERSBeamline::vespers()->sampleStageMotorSet()->toList().mid(0, 3));

	QGridLayout *arrowLayout = new QGridLayout;
	arrowLayout->addWidget(goUp_, 0, 1);
	arrowLayout->addWidget(goDown_, 2, 1);
	arrowLayout->addWidget(goLeft_, 1, 0);
	arrowLayout->addWidget(goRight_, 1, 2);
	arrowLayout->addWidget(stop, 1, 1);

	QHBoxLayout *jogAndStatusLayout = new QHBoxLayout;
	jogAndStatusLayout->addWidget(status_, 0, Qt::AlignLeft);
	jogAndStatusLayout->addWidget(jog_);

	QVBoxLayout *holderLayout = new QVBoxLayout;
	holderLayout->addLayout(arrowLayout);
	holderLayout->addLayout(jogAndStatusLayout);
	holderLayout->addLayout(hLayout);
	holderLayout->addLayout(vLayout);

	setLayout(holderLayout);
}

void VESPERSSampleStageView::onUpClicked()
{
	sampleStage_->moveVertical(sampleStage_->verticalPosition() + jog_->value());
}

void VESPERSSampleStageView::onDownClicked()
{
	sampleStage_->moveVertical(sampleStage_->verticalPosition() - jog_->value());
}

void VESPERSSampleStageView::onLeftClicked()
{
	sampleStage_->moveHorizontal(sampleStage_->horizontalPosition() - jog_->value());
}

void VESPERSSampleStageView::onRightClicked()
{
	sampleStage_->moveHorizontal(sampleStage_->horizontalPosition() + jog_->value());
}

void VESPERSSampleStageView::onMovingChanged(bool isMoving)
{
	if (isMoving)
		status_->setPixmap(QIcon(":/ON.png").pixmap(25));
	else
		status_->setPixmap(QIcon(":/OFF.png").pixmap(25));

	for (int i = 0; i < 4; i++)
		buttons_->button(i)->setDisabled(isMoving);
}

void VESPERSSampleStageView::onHorizontalSetpoint()
{
	sampleStage_->moveHorizontal(horizontal_->text().toDouble());
}

void VESPERSSampleStageView::onVerticalSetpoint()
{
	sampleStage_->moveVertical(vertical_->text().toDouble());
}

void VESPERSSampleStageView::onHorizontalChanged(double val)
{
	horizontal_->setText(QString::number(val, 'f', 3));
}

void VESPERSSampleStageView::onVerticalChanged(double val)
{
	vertical_->setText(QString::number(val, 'f', 3));
}

void VESPERSSampleStageView::onHorizontalMoveError(bool direction)
{
	if (direction && goRight_->isEnabled()){

		goRight_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the horizontal motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goLeft_->isEnabled()){

		goLeft_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the horizontal motor.  Consider changing the position of your sample on the sample mount."));
	}
}

void VESPERSSampleStageView::onVerticalMoveError(bool direction)
{
	if (direction && goDown_->isEnabled()){

		goDown_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the vertical motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goUp_->isEnabled()){

		goUp_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the vertical motor.  Consider changing the position of your sample on the sample mount."));
	}
}

void VESPERSSampleStageView::onNormalMoveError(bool direction)
{
	if (direction && goDown_->isEnabled()){

		goDown_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the normal motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goUp_->isEnabled()){

		goUp_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the normal motor.  Consider changing the position of your sample on the sample mount."));
	}
}

