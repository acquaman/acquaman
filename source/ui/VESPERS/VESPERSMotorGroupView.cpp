#include "VESPERSMotorGroupView.h"

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringBuilder>
#include <QAction>
#include <QMenu>

VESPERSMotorGroupView::VESPERSMotorGroupView(VESPERSMotorGroup *motorGroup, QWidget *parent)
	: QWidget(parent)
{
	motorGroup_ = motorGroup;
	currentMotorGroupInfo_ = motorGroup_->motorGroupInfo("Sample Stage - H and V");

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QFont font(this->font());
	font.setBold(true);

	titleLabel_ = new QLabel("Sample Stage");
	titleLabel_->setFont(font);
	firstPrefixLabel_ = new QLabel("H:");
	firstPrefixLabel_->setFont(font);
	secondPrefixLabel_ = new QLabel("V:");
	secondPrefixLabel_->setFont(font);
	QLabel *jog = new QLabel("Jog :");
	jog->setFont(font);

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

	stopButton_ = new QToolButton;
	stopButton_->setIcon(QIcon(":/stop.png"));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(onStopClicked()));

	QGridLayout *arrowLayout = new QGridLayout;
	arrowLayout->addWidget(goUp_, 0, 1);
	arrowLayout->addWidget(goDown_, 2, 1);
	arrowLayout->addWidget(goLeft_, 1, 0);
	arrowLayout->addWidget(goRight_, 1, 2);
	arrowLayout->addWidget(stopButton_, 1, 1);
	arrowLayout->addWidget(status_, 0, 0);

	jog_ = new QDoubleSpinBox;
	jog_->setSuffix(" mm");
	jog_->setSingleStep(0.001);
	jog_->setMinimum(0.0000);
	jog_->setValue(0.050);
	jog_->setDecimals(3);
	jog_->setAlignment(Qt::AlignCenter);
	jog_->setFixedWidth(110);

	QHBoxLayout *jogLayout = new QHBoxLayout;
	jogLayout->addWidget(jog, 0, Qt::AlignRight);
	jogLayout->addWidget(jog_);

	firstControlSetpoint_ = new QDoubleSpinBox;
	firstControlSetpoint_->setSuffix(" mm");
	firstControlSetpoint_->setSingleStep(0.001);
	firstControlSetpoint_->setRange(-100, 100);
	firstControlSetpoint_->setDecimals(3);
	firstControlSetpoint_->setAlignment(Qt::AlignCenter);
	firstControlSetpoint_->setFixedWidth(110);
	connect(firstControlSetpoint_, SIGNAL(editingFinished()), this, SLOT(onFirstControlSetpoint()));

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(firstPrefixLabel_, 0, Qt::AlignRight);
	hLayout->addWidget(firstControlSetpoint_);

	secondControlSetpoint_ = new QDoubleSpinBox;
	secondControlSetpoint_->setSuffix(" mm");
	secondControlSetpoint_->setSingleStep(0.001);
	secondControlSetpoint_->setRange(-100, 100);
	secondControlSetpoint_->setDecimals(3);
	secondControlSetpoint_->setAlignment(Qt::AlignCenter);
	secondControlSetpoint_->setFixedWidth(110);
	connect(secondControlSetpoint_, SIGNAL(editingFinished()), this, SLOT(onSecondControlSetpoint()));

	QHBoxLayout *vLayout = new QHBoxLayout;
	vLayout->addWidget(secondPrefixLabel_, 0, Qt::AlignRight);
	vLayout->addWidget(secondControlSetpoint_);

	QVBoxLayout *absoluteValueLayout = new QVBoxLayout;
	absoluteValueLayout->addLayout(hLayout);
	absoluteValueLayout->addLayout(vLayout);
	absoluteValueLayout->addLayout(jogLayout);

	QHBoxLayout *motorGroupLayout = new QHBoxLayout;
	motorGroupLayout->addLayout(arrowLayout);
	motorGroupLayout->addLayout(absoluteValueLayout);

	QVBoxLayout *fullLayout = new QVBoxLayout;
	fullLayout->addWidget(titleLabel_);
	fullLayout->addLayout(motorGroupLayout);

	setLayout(fullLayout);
}

void VESPERSMotorGroupView::onUpClicked()
{
	if (currentMotorGroupInfo_->secondControl())
		currentMotorGroupInfo_->secondControl()->move(secondControlSetpoint_->value() + jog_->value());

	else if (currentMotorGroupInfo_->orientation() == VESPERSMotorGroupInfo::Vertical)
		currentMotorGroupInfo_->firstControl()->move(firstControlSetpoint_->value() + jog_->value());
}

void VESPERSMotorGroupView::onDownClicked()
{
	if (currentMotorGroupInfo_->secondControl())
		currentMotorGroupInfo_->secondControl()->move(secondControlSetpoint_->value() - jog_->value());

	else if (currentMotorGroupInfo_->orientation() == VESPERSMotorGroupInfo::Vertical)
		currentMotorGroupInfo_->firstControl()->move(firstControlSetpoint_->value() - jog_->value());
}

void VESPERSMotorGroupView::onLeftClicked()
{
	currentMotorGroupInfo_->firstControl()->move(firstControlSetpoint_->value() - jog_->value());
}

void VESPERSMotorGroupView::onRightClicked()
{
	currentMotorGroupInfo_->firstControl()->move(firstControlSetpoint_->value() + jog_->value());
}

void VESPERSMotorGroupView::onStopClicked()
{
	currentMotorGroupInfo_->firstControl()->stop();

	if (currentMotorGroupInfo_->secondControl())
		currentMotorGroupInfo_->secondControl()->stop();
}

void VESPERSMotorGroupView::onMovingChanged()
{
	bool isMoving = currentMotorGroupInfo_->firstControl()->isMoving();

	if (currentMotorGroupInfo_->secondControl())
		isMoving |= currentMotorGroupInfo_->secondControl()->isMoving();

	status_->setPixmap(QIcon(isMoving ? ":/ON.png" : ":/OFF.png").pixmap(25));

	goUp_->setDisabled(isMoving);
	goDown_->setDisabled(isMoving);
	goLeft_->setDisabled(isMoving);
	goRight_->setDisabled(isMoving);
}

void VESPERSMotorGroupView::onFirstControlSetpoint()
{
	currentMotorGroupInfo_->firstControl()->move(firstControlSetpoint_->value());
}

void VESPERSMotorGroupView::onSecondControlSetpoint()
{
	// This is safe because this should only be invoked if there was a valid second control in the first place.
	currentMotorGroupInfo_->secondControl()->move(secondControlSetpoint_->value());
}

void VESPERSMotorGroupView::setMotorGroupInfo(VESPERSMotorGroupInfo *info)
{
	// Cleanup
	disconnect(currentMotorGroupInfo_->firstControl(), SIGNAL(valueChanged(double)), firstControlSetpoint_, SLOT(setValue(double)));
	disconnect(currentMotorGroupInfo_->firstControl(), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged()));

	if (currentMotorGroupInfo_->secondControl()){

		disconnect(currentMotorGroupInfo_->secondControl(), SIGNAL(valueChanged(double)), secondControlSetpoint_, SLOT(setValue(double)));
		disconnect(currentMotorGroupInfo_->secondControl(), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged()));
	}

	goUp_->hide();
	goDown_->hide();
	goLeft_->hide();
	goRight_->hide();
	secondPrefixLabel_->hide();
	secondControlSetpoint_->hide();

	// Get everything looking right for the new info.
	currentMotorGroupInfo_ = info;

	titleLabel_->setText(currentMotorGroupInfo_->name());
	firstPrefixLabel_->setText(currentMotorGroupInfo_->firstPrefix() % ":");
	firstControlSetpoint_->setSuffix(" " % currentMotorGroupInfo_->units());
	firstControlSetpoint_->setValue(currentMotorGroupInfo_->firstControl()->value());
	connect(currentMotorGroupInfo_->firstControl(), SIGNAL(valueChanged(double)), firstControlSetpoint_, SLOT(setValue(double)));
	connect(currentMotorGroupInfo_->firstControl(), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged()));

	if (currentMotorGroupInfo_->secondControl()){

		secondPrefixLabel_->setText(info->secondPrefix() % ":");
		secondControlSetpoint_->setSuffix(" " % currentMotorGroupInfo_->units());
		secondControlSetpoint_->setValue(currentMotorGroupInfo_->secondControl()->value());
		connect(currentMotorGroupInfo_->secondControl(), SIGNAL(valueChanged(double)), secondControlSetpoint_, SLOT(setValue(double)));
		connect(currentMotorGroupInfo_->secondControl(), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged()));

		goUp_->show();
		goDown_->show();
		goLeft_->show();
		goRight_->show();
		secondPrefixLabel_->show();
		secondControlSetpoint_->show();
	}

	else if (currentMotorGroupInfo_->orientation() == VESPERSMotorGroupInfo::Horizontal){

		goLeft_->show();
		goRight_->show();
	}

	else if (currentMotorGroupInfo_->orientation() == VESPERSMotorGroupInfo::Vertical){

		goUp_->show();
		goDown_->show();
	}

	jog_->setValue(0.05);
	jog_->setSuffix(" " % currentMotorGroupInfo_->units());
}

void VESPERSMotorGroupView::onCustomContextMenuRequested(const QPoint &pos)
{
	QMenu popup(this);
	QAction *temp;
	QList<QString> names = motorGroup_->names();

	for (int i = 0, size = names.size(); i < size; i++)
		popup.addAction(names.at(i));

	temp = popup.exec(mapToGlobal(pos));

	if (temp)
		setMotorGroupInfo(motorGroup_->motorGroupInfo(temp->text()));
}
