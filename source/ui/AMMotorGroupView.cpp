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
 AMMotorGroupObjectView::~AMMotorGroupObjectView(){}
AMMotorGroupObjectView::AMMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent)
	: QWidget(parent)
{
	motorGroupObject_ = motorGroupObject;

	QFont font(this->font());
	font.setBold(true);

	titleLabel_ = new QLabel(motorGroupObject->name());
	titleLabel_->setFont(font);

	foreach (QString prefix, motorGroupObject_->prefixes()){

		QLabel *label = new QLabel(prefix % " :");
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

		goIn_->setIcon(QIcon(":/go-out.png"));
		goOut_->setIcon(QIcon(":/go-in.png"));
	}

	else{

		goIn_->setIcon(QIcon(":/ArrowCCW.png"));
		goOut_->setIcon(QIcon(":/ArrowCW.png"));
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

	QStringList units = motorGroupObject_->units();
	units.removeDuplicates();
	QString unitString = "";

	foreach (QString temp, units)
		unitString = unitString % temp % "/";

	unitString.chop(1);

	jog_ = new QDoubleSpinBox;
	jog_->setSuffix(" " % unitString);
	jog_->setSingleStep(0.001);
	jog_->setMinimum(0.0000);
	jog_->setValue(0.050);
	jog_->setDecimals(3);
	jog_->setAlignment(Qt::AlignCenter);
	jog_->setFixedWidth(110);

	QHBoxLayout *jogLayout = new QHBoxLayout;
	jogLayout->addWidget(jog, 0, Qt::AlignRight);
	jogLayout->addWidget(jog_, 0, Qt::AlignRight);

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
		hLayout->addWidget(setpoint, 0, Qt::AlignRight);

		absoluteValueLayout->addLayout(hLayout);
	}

	absoluteValueLayout->addLayout(jogLayout);

	for (int i = 0, size = controlSetpoints_.size(); i < size; i++){

		connect(motorGroupObject_->controlAt(i), SIGNAL(valueChanged(double)), controlSetpoints_.at(i), SLOT(setValue(double)));
		connect(motorGroupObject_->controlAt(i), SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged()));
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
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() - jog_->value());
}

void AMMotorGroupObjectView::onRightClicked()
{
	int index = motorGroupObject_->horizontalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() + jog_->value());
}

void AMMotorGroupObjectView::onOutClicked()
{
	int index = motorGroupObject_->normalIndex();
	motorGroupObject_->controlAt(index)->move(controlSetpoints_.at(index)->value() + jog_->value());
}

void AMMotorGroupObjectView::onInClicked()
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
 AMMotorGroupView::~AMMotorGroupView(){}
AMMotorGroupView::AMMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent)
	: QWidget(parent)
{
	viewMode_ = Exclusive;
	motorGroup_ = motorGroup;

	availableMotorGroupObjects_ = new QComboBox;

	foreach(QString name, motorGroup_->names()){

		availableMotorGroupObjects_->addItem(name);
		motorGroupViews_.insert(name, new AMMotorGroupObjectView(motorGroup_->motorGroupObject(name)));
	}

	foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
		view->setTitleVisible(false);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	QVBoxLayout *groupLayout = new QVBoxLayout;
	groupLayout->addWidget(availableMotorGroupObjects_, 0, Qt::AlignLeft);

	foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
		groupLayout->addWidget(view);

	setLayout(groupLayout);
}

AMMotorGroupView::AMMotorGroupView(AMMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent)
	: QWidget(parent)
{
	viewMode_ = viewMode;
	motorGroup_ = motorGroup;

	availableMotorGroupObjects_ = new QComboBox;

	foreach(QString name, motorGroup_->names()){

		availableMotorGroupObjects_->addItem(name);
		motorGroupViews_.insert(name, new AMMotorGroupObjectView(motorGroup_->motorGroupObject(name)));
	}

	if (viewMode_ == Exclusive)
		foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
			view->setTitleVisible(false);

	else if (viewMode_ == Multiple){

		availableMotorGroupObjects_->hide();

		foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
			view->hide();
	}

	foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
		view->setTitleVisible(false);

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(availableMotorGroupObjects_, SIGNAL(currentIndexChanged(QString)), this, SLOT(setMotorGroupView(QString)));

	QVBoxLayout *groupLayout = new QVBoxLayout;
	groupLayout->addWidget(availableMotorGroupObjects_, 0, Qt::AlignLeft);

	foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
		groupLayout->addWidget(view);

	setLayout(groupLayout);
}

void AMMotorGroupView::onCustomContextMenuRequested(const QPoint &pos)
{
	QMenu popup(this);
	QAction *temp;

	buildStandardMenuItems(&popup);

	temp = popup.exec(mapToGlobal(pos));

	if (temp)
		handleStandardMenuItems(temp->text());
}

void AMMotorGroupView::handleStandardMenuItems(const QString &command)
{
	if (command.contains("Switch to Multiple"))
		setViewMode(Multiple);

	else if (command.contains("Switch to Exclusive"))
		setViewMode(Exclusive);

	else
		setMotorGroupView(command);
}

void AMMotorGroupView::setMotorGroupView(const QString &name)
{
	if (!motorGroupViews_.contains(name))
		return;

	if (viewMode_ == Exclusive){

		foreach(AMMotorGroupObjectView *view, motorGroupViews_.values())
			view->hide();

		currentMotorGroupObjectView_ = motorGroupViews_.value(name);
		motorGroupViews_.value(name)->show();

		if (availableMotorGroupObjects_->currentIndex() != availableMotorGroupObjects_->findText(name)){

			availableMotorGroupObjects_->blockSignals(true);
			availableMotorGroupObjects_->setCurrentIndex(availableMotorGroupObjects_->findText(name));
			availableMotorGroupObjects_->blockSignals(false);
		}

		emit currentMotorGroupObjectViewChanged(name);
	}

	else if (viewMode_ == Multiple){

		AMMotorGroupObjectView *view = motorGroupViews_.value(name);

		if (visibleMotorGroupObjectViews_.contains(name))
			visibleMotorGroupObjectViews_.remove(name);

		else
			visibleMotorGroupObjectViews_.insert(name, view);

		view->setVisible(!view->isVisible());
		emit motorGroupVisibilityChanged(name);
	}
}

void AMMotorGroupView::setViewMode(ViewMode mode)
{
	if (viewMode_ != mode){

		viewMode_ = mode;

		availableMotorGroupObjects_->setVisible(viewMode_ == Exclusive);

		// Since Multiple view is the only one currently that shows the titles, I'm cheating by checking only against it.
		foreach (AMMotorGroupObjectView *view, motorGroupViews_.values()){

			view->setTitleVisible(viewMode_ == Multiple);
			view->hide();
		}

		if (viewMode_ == Exclusive){

			currentMotorGroupObjectView_->show();
			emit currentMotorGroupObjectViewChanged(currentMotorGroupObjectView_->motorGroupObject()->name());
		}

		else if (viewMode_ == Multiple){

			// This is just in case there isn't a selected view yet.
			if (visibleMotorGroupObjectViews_.isEmpty()){

				visibleMotorGroupObjectViews_.insert(currentMotorGroupObjectView_->motorGroupObject()->name(), currentMotorGroupObjectView_);
				currentMotorGroupObjectView_->show();
			}

			else
				foreach (AMMotorGroupObjectView *view, visibleMotorGroupObjectViews_.values())
					view->show();

			emit motorGroupVisibilityChanged("");
		}
	}
}

void AMMotorGroupView::buildStandardMenuItems(QMenu *menu)
{
	if (viewMode_ == Exclusive){

		menu->addAction("Switch to Multiple View");
	}

	else if (viewMode_ == Multiple){

		menu->addAction("Switch to Exclusive View");
		menu->addSeparator();

		for (int i = 0, size = motorGroupViews_.size(); i < size; i++){

			QAction *action = new QAction(motorGroupViews_.keys().at(i), menu);
			action->setCheckable(true);
			action->setChecked(motorGroupViews_.values().at(i)->isVisible());
			menu->addAction(action);
		}
	}
}

QString AMMotorGroupView::currentMotorGroupObjectName() const
{
	return viewMode_ == Exclusive ? currentMotorGroupObjectView_->motorGroupObject()->name() : "";
}

AMMotorGroupObjectView *AMMotorGroupView::currentMotorGroupObjectView() const
{
	return viewMode_ == Exclusive ? currentMotorGroupObjectView_ : 0;
}

QStringList AMMotorGroupView::visibleMotorGroupObjectNames() const
{
	if (viewMode_ == Multiple){

		QStringList visibleNames;

		foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
			if (view->isVisible())
				visibleNames << view->motorGroupObject()->name();

		return visibleNames;
	}

	else
		return QStringList();
}

QList<AMMotorGroupObjectView *> AMMotorGroupView::visibleMotorGroupObjectViews() const
{
	if (viewMode_ == Multiple){

		QList<AMMotorGroupObjectView *> visibleViews;

		foreach (AMMotorGroupObjectView *view, motorGroupViews_.values())
			if (view->isVisible())
				visibleViews << view;

		return visibleViews;
	}

	else
		return QList<AMMotorGroupObjectView *>();
}
