#ifndef AMLINEEDITDIALOG_H
#define AMLINEEDITDIALOG_H

#include <QDialog>

#include <QLineEdit>

/// This class is a simple dialog with a question and line edit for response.
class AMLineEditDialog : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.  Builds the dialog.
	AMLineEditDialog(const QString &title, const QString &question, const QString &defaultAnswer = "", QWidget *parent = 0);
	/// Destructor.
	virtual ~AMLineEditDialog(){}

	/// Returns the contents of the line edit.
	QString answer() const { return answerLineEdit_->text(); }

	/// Static member.  Builds the dialog and returns the contents of the line edit.  If cancelled, returns the default answer.
	static QString retrieveAnswer(const QString &title, const QString &question, const QString &defaultAnswer = "", QWidget *parent = 0);

protected:

	/// The line edit pointer.
	QLineEdit *answerLineEdit_;
	/// The default answer.
	QString defaultAnswer_;
};

#endif // AMLINEEDITDIALOG_H
