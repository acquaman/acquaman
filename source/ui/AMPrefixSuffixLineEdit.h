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


#ifndef AMPREFIXSUFFIXLINEEDIT_H
#define AMPREFIXSUFFIXLINEEDIT_H

#include <QLineEdit>
#include <QValidator>

namespace AMPrefixSuffix {
	static QChar separator1() { return QChar('\02'); }
	static QChar separator2() { return QChar('\03'); }
}

/// The AMPrefixSuffixLineEdit is a tenacious hack whereby we use non-printing characters within the line edit to separate the user's actual text from the fixed prefix and suffix. We also implement a validator on the line edit so that these suffix and prefix regions cannot be deleted or edited. This class implements that validator.
class AMPrefixSuffixValidator : public QValidator {
public:
	// Fortunately, this function gets called every single time the line edit is typed into. We use it to intercept and negate any editing of the prefix or suffix.
	State validate ( QString & input, int & pos ) const;

	void fixup ( QString & input ) const {
		Q_UNUSED(input);
	}

	void setPrefix(const QString& prefix) {
		prefix_ = prefix;
		// actually changing the text is handled within the line-edit class
	}
	void setSuffix(const QString& suffix) {
		suffix_ = suffix;
	}

protected:
	QString prefix_, suffix_;

};

/// This class implements a standard QLineEdit that displays a fixed, non-editable prefix and/or suffix.  You can use it just like a QLineEdit, calling setText() and text() with the actual internal text value.  Use setPrefix() and setSuffix() to change what is displayed in front and after this text.  Use fullText() to retrieve the complete string, including the prefix and the suffix.
class AMPrefixSuffixLineEdit : public QLineEdit
{
	Q_OBJECT
public:


 	virtual ~AMPrefixSuffixLineEdit();
	AMPrefixSuffixLineEdit(const QString& prefix = QString(), const QString& suffix = QString(), QWidget *parent = 0);

	QString text() const {
		QString ft = fullText();

		if(ft.contains(AMPrefixSuffix::separator1()))
			ft.remove(0, ft.indexOf(AMPrefixSuffix::separator1()) + 1);

		if(ft.contains(AMPrefixSuffix::separator2()))
			ft.chop(ft.length() - ft.lastIndexOf(AMPrefixSuffix::separator2()));

		return ft;
	}

	QString fullText() const {
		return QLineEdit::text();
	}

	void setText(const QString & newText) {
		QLineEdit::setText(prefix_ + AMPrefixSuffix::separator1() + newText + AMPrefixSuffix::separator2() + suffix_);
	}

	// insert() will work as usual, because it will pass through the validator.

signals:

public slots:
	void setPrefix(const QString& prefix);
	void setSuffix(const QString& suffix);

protected:
	QString prefix_, suffix_,text_;
	AMPrefixSuffixValidator validator_;

};

#endif // AMPREFIXSUFFIXLINEEDIT_H
