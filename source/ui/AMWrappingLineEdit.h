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


#ifndef AMWRAPPINGLINEEDIT_H
#define AMWRAPPINGLINEEDIT_H

#pragma GCC diagnostic ignored "-Wunused-private-field"
#include <QPlainTextEdit>
#pragma GCC diagnostic warning "-Wunused-private-field"

/// This class reduces the behaviour of a QPlainTextEdit and makes it behave more like a QLineEdit. Unlike QLineEdit, wrapping of text onto multiple lines is possible. However, as soon as the Enter or Return key is pressed, the editingFinished() signal is emitted and the extra carriage return is deleted from the text.
class AMWrappingLineEdit : public QPlainTextEdit
{
Q_OBJECT
public:
	/// These values, or one of Qt::FocusReason, will be given as the reason why editing finished in the editingFinished(int) signal.
	enum FinishReason { EscapePressedReason = Qt::OtherFocusReason + 1, EnterPressedReason, UnknownReason };

 	virtual ~AMWrappingLineEdit();
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
