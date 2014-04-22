#ifndef AMCANCELACTIONPROMPT_H
#define AMCANCELACTIONPROMPT_H

#include <QDialog>

class QLabel;
class QCheckBox;

/// This is a simple QMessageBox that is extended by having a "do no warn me again" check box.
class AMCancelActionPrompt : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMCancelActionPrompt();
	AMCancelActionPrompt(QWidget *parent = 0);

	/// Sets the text for the dialog.
	void setText(const QString &newText);
	/// Returns the current text.
	const QString &text() const { return text_; }

	/// Returns whether the warning check box is checked or not.
	bool shouldWarn() const;

protected:
	/// The string that holds the text.
	QString text_;
	/// The label holding for the text.
	QLabel *textLabel_;
	/// The check box for "do not warn me again".
	QCheckBox *warningCheckBox_;
};

#endif // AMCANCELACTIONPROMPT_H
