#include "BioXASPseudoMotorControlEditor.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>

BioXASPseudoMotorControlEditor::BioXASPseudoMotorControlEditor(BioXASPseudoMotorControl* control, AMControl* statusTagControl, bool readOnly, bool configureOnly, QWidget *parent)
	:AMExtendedControlEditor(control, statusTagControl, readOnly, configureOnly, parent)
{
	valueLabel_->setToolTip(control->readPVName());
	unitsLabel_->setToolTip(control->readPVName() + ".EGU");
	statusLabel_->setToolTip(control->statusPVName());

	// setup the enabled status dispaly
	QFont statusFont;
	statusFont.setPointSize(10);

	enabledStatusLabel_ = new QLabel("[NOT CONNECTED]");
	enabledStatusLabel_->setToolTip(control->enabledPVName());
	statusLayout_->addWidget(enabledStatusLabel_);

	connect(control, SIGNAL(enabledPVValueChanged(double)), this, SLOT(onEnabledPVStatusChanged(double)));
	onEnabledPVStatusChanged(control->enabledPVStatus());

	// setup context menu for stopping a motor
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onShowContextMenu(const QPoint&)));
}

void BioXASPseudoMotorControlEditor::onShowContextMenu(const QPoint& pos)
{
	BioXASPseudoMotorControl * bioxasPseudoMotor = (BioXASPseudoMotorControl *)control_;

	QMenu contextMenu;
	contextMenu.addAction("Stop " + bioxasPseudoMotor->pvBaseName());
	QAction* selectedItem = contextMenu.exec(mapToGlobal(pos));
	if (selectedItem) {
		if (bioxasPseudoMotor->stop())
			qDebug() << "BioXAX motor "+ bioxasPseudoMotor->pvBaseName() + ":stop is stopped.";
		else
			qDebug() << "Failed to stop BioXAX motor "+ bioxasPseudoMotor->pvBaseName() + ":stop.";
	}
}

void BioXASPseudoMotorControlEditor::onEnabledPVStatusChanged(double value)
{
	if (value == -1)
		enabledStatusLabel_->setText("NOT CONNECTED");
	else if (value == 1)
		enabledStatusLabel_->setText("ENABLED");
	else
		enabledStatusLabel_->setText("DISABLED");

	updateReadOnlyStatus();
}

