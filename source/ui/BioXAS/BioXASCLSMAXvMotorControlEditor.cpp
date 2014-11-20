#include "BioXASCLSMAXvMotorControlEditor.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QDebug>

BioXASCLSMAXvMotorControlEditor::BioXASCLSMAXvMotorControlEditor(CLSMAXvMotor* control, AMControl* statusTagControl, bool readOnly, bool configureOnly, QWidget *parent)
	:AMExtendedControlEditor(control, statusTagControl, readOnly, configureOnly, parent)
{
	valueLabel_->setToolTip(control->readPVName());
	unitsLabel_->setToolTip(control->readPVName() + " (units)");
	statusLabel_->setToolTip(control->statusPVName());

	QFont statusFont;
	statusFont.setPointSize(10);

	cwLabel_ = new QLabel("CW");
	cwLabel_->setFont(statusFont);
	cwLabel_->setMargin(1);
	cwLabel_->setVisible(false);
	cwLabel_->setToolTip(control->CWPVName());
	statusLayout_->addWidget(cwLabel_);

	ccwLabel_ = new QLabel("CCW");
	ccwLabel_->setFont(statusFont);
	ccwLabel_->setMargin(1);
	ccwLabel_->setVisible(false);
	ccwLabel_->setToolTip(control->CCWPVName());
	statusLayout_->addWidget(ccwLabel_);

	// setup context menu for stopping a motor
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onShowContextMenu(const QPoint&)));

	connect(control, SIGNAL(atLimitChanged(CLSMAXvMotor::Limit)), this, SLOT(onLimitChanged(CLSMAXvMotor::Limit)));
	if (control->isConnected()) {
		onLimitChanged(control->atLimit());
	}
}

BioXASCLSMAXvMotorControlEditor::~BioXASCLSMAXvMotorControlEditor()
{

}

void BioXASCLSMAXvMotorControlEditor::onShowContextMenu(const QPoint& pos)
{
	CLSMAXvMotor * bioxasMAXvMotor = (CLSMAXvMotor *)control_;

	QMenu contextMenu;
	contextMenu.addAction("Stop " + bioxasMAXvMotor->pvBaseName());
	QAction* selectedItem = contextMenu.exec(mapToGlobal(pos));
	if (selectedItem) {
		if (bioxasMAXvMotor->stop())
			qDebug() << "BioXAX motor "+ bioxasMAXvMotor->pvBaseName() + ":stop is stopped.";
		else
			qDebug() << "Failed to stop BioXAX motor "+ bioxasMAXvMotor->pvBaseName() + ":stop.";
	}
}

void BioXASCLSMAXvMotorControlEditor::onLimitChanged(CLSMAXvMotor::Limit limit)
{
	switch (limit) {
	case CLSMAXvMotor::LimitNone: ///< Not at a limit
		cwLabel_->setVisible(false);
		ccwLabel_->setVisible(false);
		break;
	case CLSMAXvMotor::LimitCW:   ///< At the CW limit
		cwLabel_->setVisible(true);
		break;
	case CLSMAXvMotor::LimitCCW:  ///< At the CCW limit
		ccwLabel_->setVisible(true);
		break;
	case CLSMAXvMotor::LimitError: ///< There is a limit switch error (both of them are on at the same time or not connected)
	default:
		cwLabel_->setVisible(true);
		ccwLabel_->setVisible(true);
		break;
	}
}
