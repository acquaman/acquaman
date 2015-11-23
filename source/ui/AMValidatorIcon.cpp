#include "AMValidatorIcon.h"

#include "util/AMValidator.h"

#include <QPixmap>
#include <QVBoxLayout>
AMValidatorIcon::AMValidatorIcon(AMValidator* validator,
								 const QString& iconPath,
								 QWidget *parent) :
	QWidget(parent)
{

	validator_ = validator;

	if(validator_) {

		iconLabel_ = new QLabel();
		iconLabel_->setPixmap(QPixmap(iconPath));
		iconLabel_->setToolTip(validator_->fullFailureMessage());
		QVBoxLayout* mainLayout = new QVBoxLayout();
		mainLayout->addWidget(iconLabel_);
		setLayout(mainLayout);

		connect(validator_, SIGNAL(validStateChanged(bool)), this, SLOT(setVisible(bool)));
		connect(validator_, SIGNAL(failCountChanged(int)), this, SLOT(onFailCountChanged(int)));
	}

}

void AMValidatorIcon::onFailCountChanged(int)
{
	iconLabel_->setToolTip(validator_->fullFailureMessage());
}
