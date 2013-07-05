#include "AMMotorGroupView.h"

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStringBuilder>
#include <QAction>
#include <QMenu>

// AMMotorGroupObjectView
//////////////////////////////////////////////
AMMotorGroupObjectView::AMMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent)
	: QWidget(parent)
{
	motorGroupObject_ = motorGroupObject;

	QFont font(this->font());
	font.setBold(true);

	titleLabel_ = new QLabel(motorGroupObject->name());
	titleLabel_->setFont(font);

	foreach (QString prefix, motorGroupObject_->prefixes()){

		QLabel *label = new QLabel(prefix % ":");
		label->setFont(font);
		prefixLabels_ << label;
	}

	QLabel *jog = new QLabel("Jog :");
	jog->setFont(font);

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25));

	goUp_ = new QToolButton;
	goDown_ = new QToolButton;
	connect(goUp_, SIGNAL(clicked()), this, SLOT(onUpClicked()));
	connect(goDown_, SIGNAL(clicked()), this, SLOT(onDownClicked()));

	if (motorGroupObject_->verticalIndex() == -1){

		goUp_->hide();
		goDown_->hide();
	}

	else if (motorGroupObject_->motionTypeAt(motorGroupObject_->verticalIndex()) == AMMotorGroupObject::Translational){

		goUp_->setIcon(QIcon(":/go-up.png"));
		goDown_->setIcon(QIcon(":/go-down.png"));
	}

	else{

		goUp_->setIcon(QIcon(":/ArrowCW.png"));
		goDown_->setIcon(QIcon(":/ArrowCCW.png"));
	}

	goLeft_ = new QToolButton;
	goRight_ = new QToolButton;
	connect(goLeft_, SIGNAL(clicked()), this, SLOT(onLeftClicked()));
	connect(goRight_, SIGNAL(clicked()), this, SLOT(onRightClicked()));

	if (motorGroupObject_->horizontalIndex() == -1){

		goLeft_->hide();
		goRight_->hide();
	}

	else if (motorGroupObject_->motionTypeAt(motorGroupObject_->horizontalIndex()) == AMMotorGroupObject::Translational){

		goLeft_->setIcon(QIcon(":/go-previous.png"));
		goRight_->setIcon(QIcon(":/go-next.png"));
	}

	else{

		goLeft_->setIcon(QIcon(":/ArrowCW.png"));
		goRight_->setIcon(QIcon(":/ArrowCCW.png"));
	}

	goIn_ = new QToolButton;
	goOut_ = new QToolButton;
	connect(goIn_, SIGNAL(clicked()), this, SLOT(onInClicked()));
	connect(goOut_, SIGNAL(clicked()), this, SLOT(onOutClicked()));

	if (motorGroupObject_->normalIndex() == -1){

		goIn_->hide();
		goOut_->hide();
	}

	else if (motorGroupObject_->motionTypeAt(motorGroupObject_->normalIndex()) == AMMotorGroupObject::Translational){

		goIn_->setIcon(QIcon(":/go-in.png"));
		goOut_->setIcon(QIcon(":/go-out.png"));
	}

	else{

		goIn_->setIcon(QIcon(":/ArrowCW.png"));
		goOut_->setIcon(QIcon(":/ArrowCCW.png"));
	}

	stopButton_ = new QToolButton;
	stopButton_->setIcon(QIcon(":/stop.png"));
	connect(stopButton_, SIGNAL(clicked()), this, SLOT(onStopClicked()));

	QGridLayout *arrowLayout = new QGridLayout;
	arrowLayout->addWidget(goUp_, 0, 1);
	arrowLayout->addWidget(goDown_, 2, 1);
	arrowLayout->addWidget(goLeft_, 1, 0);
	arrowLayout->addWidget(goRight_, 1, 2);
	arrowLayout->addWidget(goIn_, 2, 0);
	arrowLayout->addWidget(goOut_, 0, 2);
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

	QVBoxLayout *absoluteValueLayout = new QVBoxLayout;

	for (int i = 0, size = motorGroupObject_->size(); i < size; i++){

		QDoubleSpinBox *setpoint = new QDoubleSpinBox;
		setpoint->setSuffix(" " % motorGroupObject_->unitAt(i));
		setpoint->setSingleStep(0.001);
		setpoint->setRange(-100, 100);
		setpoint->setDecimals(3);
		setpoint->setAlignment(Qt::AlignCenter);
		setpoint->setFixedWidth(110);
		controlSetpoints_ << setpoint;

		QHBoxLayout *hLayout = new QHBoxLayout;
		hLayout->addWidget(prefixLabels_.at(i), 0, Qt::AlignRight);
		hLayout->addWidget(setpoint);

		absoluteValueLayout->addLayout(hLayout);
	}

	absoluteValueLayout->addLayout(jogLayout);

	for (int i = 0, size = controlSetpoints_.size(); i < size; i++){

		connect(motorGroupObject_->controlAt(i), SIGNAL(valueChanged(double)), controlSetpoints_.at(i), SLOT(setValue(double)));
		connect(motorGroupObject_->controlAt(i), SIGNAL(movingChanged(double)), this, SLOT(onMovingChanged()));
	}

	connect(controlSetpoints_.at(0), SIGNAL(editingFinished()), this, SLOT(onFirstControlSetpoint()));

	if (controlSetpoints_.size() > 1)
		connect(controlSetpoints_.at(1), SIGNAL(editingFinished()), this, SLOT(onSecondControlSetpoint()));

	if (controlSetpoints_.size() > 2)
		connect(controlSetpoints_.at(2), SIGNAL(editingFinished()), this, SLOT(onThirdControlSetpoint()));

	QHBoxLayout *motorGroupLayout = new QHBoxLayout;
	motorGroupLayout->addLayout(arrowLayout);
	motorGroupLayout->addLayout(absoluteValueLayout);

	QVBoxLayout *fullLayout = new QVBoxLayout;
	fullLayout->addWidget(titleLabel_);
	fullLayout->addLayout(motorGroupLayout);

	setLayout(fullLayout);
}

void AMMotorGroupObjectView::onUpClicked()
{
	int index = motorGroupObject_->verticalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() + jog_->value());
}

void AMMotorGroupObjectView::onDownClicked()
{
	int index = motorGroupObject_->verticalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() - jog_->value());
}

void AMMotorGroupObjectView::onLeftClicked()
{
	int index = motorGroupObject_->horizontalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() + jog_->value());
}

void AMMotorGroupObjectView::onRightClicked()
{
	int index = motorGroupObject_->horizontalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() - jog_->value());
}

void AMMotorGroupObjectView::onInClicked()
{
	int index = motorGroupObject_->normalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() + jog_->value());
}

void AMMotorGroupObjectView::onOutClicked()
{
	int index = motorGroupObject_->normalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() - jog_->value());
}

void AMMotorGroupObjectView::onStopClicked()
{
	foreach (AMControl *control, motorGroupObject_->controls())
		control->stop();
}

void AMMotorGroupObjectView::onMovingChanged()
{
	bool isMoving = true;

	foreach (AMControl *control, motorGroupObject_->controls())
		isMoving &= control->isMoving();

	status_->setPixmap(QIcon(isMoving ? ":/ON.png" : ":/OFF.png").pixmap(25));

	goUp_->setDisabled(isMoving);
	goDown_->setDisabled(isMoving);
	goLeft_->setDisabled(isMoving);
	goRight_->setDisabled(isMoving);
	goIn_->setDisabled(isMoving);
	goOut_->setDisabled(isMoving);
}

void AMMotorGroupObjectView::onFirstControlSetpoint()
{
	motorGroupObject_->controlAt(0)->move(controlSetpoints_.at(0)->value());
}

void AMMotorGroupObjectView::onSecondControlSetpoint()
{
	motorGroupObject_->controlAt(1)->move(controlSetpoints_.at(1)->value());
}

void AMMotorGroupObjectView::onThirdControlSetpoint()
{
	motorGroupObject_->controlAt(2)->move(controlSetpoints_.at(2)->value());
}

// AMMotorGroupView
/////////////////////////////////////////////
AMMotorGroupView::AMMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent)
	: QWidget(parent)
{
	motorGroup_ = motorGroup;

	foreach(QString name, motorGroup_->names())
		motorGroupViews_.insert(name, new AMMotorGroupObjectView(motorGroup_->motorGroupObject(name)));

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);
}

void AMMotorGroupView::onCustomContextMenuRequested(const QPoint &pos)
{
	QMenu popup(this);
	QAction *temp;
	QList<QString> names = motorGroupViews_.keys();

	for (int i = 0, size = names.size(); i < size; i++)
		popup.addAction(names.at(i));

	temp = popup.exec(mapToGlobal(pos));

	if (temp && motorGroupViews_.value(temp->text())){

		// This loop is not efficient.  I'm sure there is a better way to deal with this.
		foreach(AMMotorGroupObjectView *view, motorGroupViews_.values())
			view->hide();

		motorGroupViews_.value(temp->text())->show();
	}
}
