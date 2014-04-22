#include "AMCancelActionPrompt.h"

#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

 AMCancelActionPrompt::~AMCancelActionPrompt(){}
AMCancelActionPrompt::AMCancelActionPrompt(QWidget *parent)
	: QDialog(parent)
{
	setModal(true);

	QLabel *questionMarkLabel = new QLabel;
	questionMarkLabel->setPixmap(QIcon(":/22x22/dialog-question.png").pixmap(22));

	textLabel_ = new QLabel;
	textLabel_->setWordWrap(true);
	warningCheckBox_ = new QCheckBox("Do no warn me again.");

	QPushButton *okayButton = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");
	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");

	connect(okayButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *textLayout = new QHBoxLayout;
	textLayout->addWidget(questionMarkLabel);
	textLayout->addWidget(textLabel_);

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(warningCheckBox_);
	buttonLayout->addStretch();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(textLayout);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}

void AMCancelActionPrompt::setText(const QString &newText)
{
	textLabel_->setText(newText);
}

bool AMCancelActionPrompt::shouldWarn() const
{
	return !warningCheckBox_->isChecked();
}
