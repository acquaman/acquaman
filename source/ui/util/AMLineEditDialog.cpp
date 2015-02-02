#include "AMLineEditDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

AMLineEditDialog::AMLineEditDialog(const QString &title, const QString &question, const QString &defaultAnswer, QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(title);

	defaultAnswer_ = defaultAnswer;

	QLabel *questionLabel = new QLabel(question);
	answerLineEdit_ = new QLineEdit;
	answerLineEdit_->setText(defaultAnswer_);
	connect(answerLineEdit_, SIGNAL(editingFinished()), this, SLOT(accept()));

	QPushButton *okayButton = new QPushButton(QIcon(":/22x22/greenCheck.png"), "Okay");
	connect(okayButton, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancelButton = new QPushButton(QIcon(":/22x22/list-remove-2.png"), "Cancel");
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(cancelButton);
	buttonLayout->addWidget(okayButton);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(questionLabel);
	layout->addWidget(answerLineEdit_);
	layout->addLayout(buttonLayout);

	setLayout(layout);
}

QString AMLineEditDialog::retrieveAnswer(const QString &title, const QString &question, const QString &defaultAnswer, QWidget *parent)
{
	AMLineEditDialog dialog(title, question, defaultAnswer, parent);
	return dialog.exec() ? dialog.answer() : defaultAnswer;
}
