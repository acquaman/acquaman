/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	warningCheckBox_ = new QCheckBox("Do not warn me again.");

	QPushButton *okayButton = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Yes");
	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "No");

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
