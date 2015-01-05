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


#include "AMSelectionDialog.h"

#include <QScrollArea>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>

 AMSelectionDialog::~AMSelectionDialog(){}
AMSelectionDialog::AMSelectionDialog(const QString &title, const QStringList &items, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(title);
	setModal(true);

	setEmptySelectedItemsAllowed(false);

	items_ = new QButtonGroup(this);
	items_->setExclusive(false);

	QVBoxLayout *itemLayout = new QVBoxLayout;

	foreach (QString name, items){

		QCheckBox *item = new QCheckBox(name);
		item->setChecked(true);
		itemLayout->addWidget(item);
		items_->addButton(item);

		connect(item, SIGNAL(stateChanged(int)), this, SLOT(onItemCheckStateChanged()));
	}

	QGroupBox *itemBox = new QGroupBox;
	itemBox->setFlat(true);
	itemBox->setLayout(itemLayout);
	QScrollArea *itemScrollArea = new QScrollArea;
	itemScrollArea->setWidget(itemBox);
	itemScrollArea->setFrameShape(QFrame::NoFrame);
	itemScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	okayButton_ = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");

	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(okayButton_, SIGNAL(clicked()), this, SLOT(accept()));

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton_);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(itemScrollArea);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);

	onItemCheckStateChanged();
}

QStringList AMSelectionDialog::selectedItems() const
{
	QStringList names;

	foreach (QAbstractButton *button, items_->buttons())
		if (button->isChecked())
			names << button->text();

	return names;
}

void AMSelectionDialog::setEmptySelectedItemsAllowed(bool allowed)
{
	emptySelectedItemsAllowed_ = allowed;
}

void AMSelectionDialog::onItemCheckStateChanged()
{
	bool okEnabled = false;
	if (emptySelectedItemsAllowed_)
		okEnabled = true;
	else {
		foreach (QAbstractButton *button, items_->buttons()) {
			if (button->isChecked()) {
				okEnabled = true;
				break;
			}
		}
	}

	okayButton_->setEnabled(okEnabled);
}
