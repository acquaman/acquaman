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

	items_ = new QButtonGroup(this);
	items_->setExclusive(false);

	QVBoxLayout *itemLayout = new QVBoxLayout;

	foreach (QString name, items){

		QCheckBox *item = new QCheckBox(name);
		item->setChecked(true);
		itemLayout->addWidget(item);
		items_->addButton(item);
	}

	QGroupBox *itemBox = new QGroupBox;
	itemBox->setFlat(true);
	itemBox->setLayout(itemLayout);
	QScrollArea *itemScrollArea = new QScrollArea;
	itemScrollArea->setWidget(itemBox);
	itemScrollArea->setFrameShape(QFrame::NoFrame);
	itemScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	QPushButton *okayButton = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");

	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(okayButton, SIGNAL(clicked()), this, SLOT(accept()));

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(itemScrollArea);
	mainLayout->addLayout(buttonLayout);

	setLayout(mainLayout);
}

QStringList AMSelectionDialog::selectedItems() const
{
	QStringList names;

	foreach (QAbstractButton *button, items_->buttons())
		if (button->isChecked())
			names << button->text();

	return names;
}
