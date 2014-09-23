#include "MAXvMotorControlEditor.h"

#include <QHBoxLayout>

MAXvMotorControlEditor::MAXvMotorControlEditor(BioXASMAXvMotor* control, AMControl* statusTagControl, bool readOnly, bool configureOnly, QWidget *parent)
	:AMExtendedControlEditor(control, statusTagControl, readOnly, configureOnly, parent)
{
	valueLabel_->setToolTip(control->feedbackPVName());
	unitsLabel_->setToolTip(control->feedbackPVName() + " (units)");
	statusLabel_->setToolTip(control->statusPVName());

        QFont statusFont;
        statusFont.setPointSize(10);

        cwLabel_ = new QLabel("CW");
        cwLabel_->setFont(statusFont);
        cwLabel_->setMargin(1);
        cwLabel_->setVisible(false);
        statusLayout_->addWidget(cwLabel_);

        ccwLabel_ = new QLabel("CCW");
        ccwLabel_->setFont(statusFont);
        ccwLabel_->setMargin(1);
        ccwLabel_->setVisible(false);
        statusLayout_->addWidget(ccwLabel_);

        connect(control, SIGNAL(atLimitChanged), this, SLOT(onLimitChanged));
}

void MAXvMotorControlEditor::onLimitChanged(CLSMAXvMotor::Limit limit)
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
