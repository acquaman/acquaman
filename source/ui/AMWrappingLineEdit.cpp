#include "AMWrappingLineEdit.h"

AMWrappingLineEdit::AMWrappingLineEdit(QWidget *parent) :
	QPlainTextEdit(parent)
{
	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
	focusOutReason_ = UnknownReason;
}


void AMWrappingLineEdit::setText(const QString &t) {
	blockSignals(true);

	QTextCursor c = textCursor();
	c.beginEditBlock();
	setPlainText(t);
	c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	c.endEditBlock();
	setTextCursor(c);

	blockSignals(false);
	// bazinga!
}

void AMWrappingLineEdit::onBlockCountChanged(int numParagraphs) {
	if(numParagraphs > 1) {
		QString contents(text());
		contents.remove(QRegExp("[\\n\\r]"));
		setText(contents);
		focusOutReason_ = EnterPressedReason;
		// this will trigger our editingFinished() signal in the focusOutEvent(), with the custom EnterPressedReason reason
		clearFocus();
	}
}


#include <QFocusEvent>

void AMWrappingLineEdit::focusOutEvent(QFocusEvent *e) {

	if(e->reason() == Qt::OtherFocusReason) {
		emit editingFinished(focusOutReason_);
		focusOutReason_ = UnknownReason;
	}
	else
		emit editingFinished(e->reason());

	QPlainTextEdit::focusOutEvent(e);
}

void AMWrappingLineEdit::keyPressEvent(QKeyEvent *e) {
	if(e->key() == Qt::Key_Escape) {
		focusOutReason_ = EscapePressedReason;
		clearFocus();
	}
	else
		QPlainTextEdit::keyPressEvent(e);
}
