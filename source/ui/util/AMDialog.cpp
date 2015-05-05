#include "AMDialog.h"

AMDialog::~AMDialog()
{

}

AMDialog::AMDialog(const QString &title, QWidget *parent) :
	QDialog(parent)
{
	setWindowTitle(title);

	contentLayout_ = new QVBoxLayout;
	buttonsLayout_ = new QHBoxLayout;

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(contentLayout_);
	layout->addLayout(buttonsLayout_);
	setLayout(layout);

	layoutDialogButtons();
}

void AMDialog::layoutDialogContent(QWidget *widget)
{
	if (widget) {
		contentLayout_->addWidget(widget);
	}
}

void AMDialog::layoutDialogButtons()
{
	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	buttonsLayout_->addWidget(buttonBox);
}


///================= AMLineEditDialog =====================
///which provides a question and will return an answer

QString AMLineEditDialog::retrieveAnswer(const QString &title, const QString &question, const QString &defaultAnswer, QWidget *parent)
{
	AMLineEditDialog dialog(title, question, defaultAnswer, parent);
	return dialog.exec() ? dialog.answer() : "";
}

AMLineEditDialog::~AMLineEditDialog()
{

}

AMLineEditDialog::AMLineEditDialog(const QString &title, const QString &question, const QString &defaultAnswer, QWidget *parent)
	: AMDialog(title, parent)
{
	question_ = question;
	defaultAnswer_ = defaultAnswer;

	layoutDialogContent();
}

/// Returns the contents of the line edit.
QString AMLineEditDialog::answer() const { return answerLineEdit_->text(); }

void AMLineEditDialog::layoutDialogContent(QWidget *widget)
{
	Q_UNUSED(widget)

	QLabel *questionLabel = new QLabel(question_);
	answerLineEdit_ = new QLineEdit;
	answerLineEdit_->setText(defaultAnswer_);

	contentLayout_->addWidget(questionLabel);
	contentLayout_->addWidget(answerLineEdit_);
}


