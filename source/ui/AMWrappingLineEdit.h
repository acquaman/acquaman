#ifndef AMWRAPPINGLINEEDIT_H
#define AMWRAPPINGLINEEDIT_H

#include <QPlainTextEdit>

/// This class reduces the behaviour of a QPlainTextEdit and makes it behave more like a QLineEdit. Unlike QLineEdit, wrapping of text onto multiple lines is possible. However, as soon as the Enter or Return key is pressed, the editingFinished() signal is emitted and the extra carriage return is deleted from the text.
class AMWrappingLineEdit : public QPlainTextEdit
{
Q_OBJECT
public:
	/// These values, or one of Qt::FocusReason, will be given as the reason why editing finished in the editingFinished(int) signal.
	enum FinishReason { EscapePressedReason = Qt::OtherFocusReason + 1, EnterPressedReason, UnknownReason };

	explicit AMWrappingLineEdit(QWidget *parent = 0);

	/// Returns the contents of the text editor. ( same as QPlainTextEdit::toPlainText() )
	QString text() const { return toPlainText(); }

signals:
	/// Emitted when the user presses Enter or Return, or leaves focus. The code is
	void editingFinished(int);
	/// Another useful signal is the QPlainTextEdit::textChanged() signal.


public slots:
	/// This setText() method accomplishes the same thing as QPlainTextEdit::setPlainText(), but it doesn't emit the textChanged() signal.
	void setText(const QString& t);

protected slots:
	/// catches a new paragraph (enter) being created, removes it, and emits editingFinished()
	void onBlockCountChanged(int numParagraphs);

protected:
	/// re-implemented from QPlainTextEdit to emit the editingFinished() signal.
	virtual void focusOutEvent(QFocusEvent *e);
	/// Re-implemented from QPlainTextEdit to emit the editingFinished() signal and give up focus when 'Escape' key is pressed
	virtual void keyPressEvent(QKeyEvent *e);

	int focusOutReason_;


};

#endif // AMWRAPPINGLINEEDIT_H
