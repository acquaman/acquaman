#ifndef AMDIALOG_H
#define AMDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QLabel>
#include <QKeyEvent>

class AMDialog : public QDialog
{
    Q_OBJECT
public:
	/// Constructor.  Builds the dialog.
	explicit AMDialog(const QString &title, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDialog();

	/// Layout the main content of the dialog
	virtual void layoutDialogContent(QWidget *widget = 0);
	/// layout the buttons of the dialog
	virtual void layoutDialogButtons();

	/// function to set the default behavior for enter key
	void setEnableDialogEnterKeyBehavior(bool enable);

protected:
	void keyPressEvent(QKeyEvent * e);

protected:
	/// flag to disable the default enter key behavior
	bool enableDefaultEnterBehavior_;

	/// the layout box to contain the content
	QVBoxLayout *contentLayout_;

	/// the layout box to contain the buttons
	QHBoxLayout *buttonsLayout_;
};

class AMLineEditDialog : public AMDialog
{
	Q_OBJECT
public:
	/// Static member.  Builds the dialog and returns the contents of the line edit.  If cancelled, returns the default answer.
	static QString retrieveAnswer(const QString &title, const QString &question, const QString &defaultAnswer = "", QWidget *parent = 0);

	/// Constructor.  Builds the dialog.
	explicit AMLineEditDialog(const QString &title, const QString &question, const QString &defaultAnswer = "", QWidget *parent = 0);
	/// Destructor.
	virtual ~AMLineEditDialog();

	/// Returns the contents of the line edit.
	QString answer() const ;

protected:
	virtual void layoutDialogContent(QWidget *widget = 0);

protected:
	/// The question
	QString question_;
	/// The default answer.
	QString defaultAnswer_;

	/// The line edit pointer.
	QLineEdit *answerLineEdit_;

};

#endif // AMDIALOG_H
