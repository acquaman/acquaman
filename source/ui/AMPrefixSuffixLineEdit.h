#ifndef AMPREFIXSUFFIXLINEEDIT_H
#define AMPREFIXSUFFIXLINEEDIT_H

#include <QLineEdit>
#include <QValidator>

namespace AMPrefixSuffix {
	static QChar separator1() { return QChar('\02'); }
	static QChar separator2() { return QChar('\03'); }
}

// The PrefixSuffix line edit is a tenacious hack whereby we use non-printing characters within the line edit to separate the user's actual text from the fixed prefix and suffix. We also implement a validator on the line edit so that these suffix and prefix regions cannot be deleted.
class AMPrefixSuffixValidator : public QValidator {
public:
	// Fortunately, this function gets called every single time the line edit is typed into. We use it to intercept and negate any editing of the prefix or suffix.
	State validate ( QString & input, int & pos ) const {

		// possible situations:
			/* - normal editing inside the allowed "actual" text area
			   - just backspaced over the prefix separator
			   - just deleted the suffix separator
			   - clicked over into the prefix or suffix regions and typed some text that shouldn't be there. (We want to capture this character and put it back where it belongs)
			   */
		// Handle all these situations by clearing the prefix and suffix (or what we think they would be, if the separators weren't deleted), and re-adding everything at the end.


		// Did they just delete the prefix separator? (ie: by hitting backspace) If so, erase everything prior to their position. (We'll re-insert the prefix and separator at the end)
		if(!input.contains(AMPrefixSuffix::separator1())) {
			input.remove(0, pos);
		}
		// otherwise, delete everything prior to and including the separator
		else {
			input.remove(0, input.indexOf(AMPrefixSuffix::separator1()) + 1);
		}

		// Did they just delete the suffix sep?
		if(!input.contains(AMPrefixSuffix::separator2())) {
			input.chop(input.length() - pos + 1);
		}
		// otherwise, delete everything after and including the separator
		else {
			input.chop(input.length() - input.lastIndexOf(AMPrefixSuffix::separator2()));
		}

		// now its clean and standard. Add back the prefix and the suffix and their separators.
		input.prepend(AMPrefixSuffix::separator1());
		input.prepend(prefix_);
		input.append(AMPrefixSuffix::separator2());
		input.append(suffix_);

		// make sure they keep their position within where it's allowed to be...
		if(pos < prefix_.length() + 1 )
			pos = prefix_.length() + 1;

		if( pos > input.length() - suffix_.length() - 1)
			pos = input.length() - suffix_.length() - 1;

		return QValidator::Acceptable;
	}

	void fixup ( QString & input ) const {
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

class AMPrefixSuffixLineEdit : public QLineEdit
{
Q_OBJECT
public:


	AMPrefixSuffixLineEdit(const QString& prefix = QString(), const QString& suffix = QString(), QWidget *parent = 0);

	QString text() const {
		QString fullText = QLineEdit::text();

		if(fullText.contains(AMPrefixSuffix::separator1()))
			fullText.remove(0, fullText.indexOf(AMPrefixSuffix::separator1()) + 1);

		if(fullText.contains(AMPrefixSuffix::separator2()))
			fullText.chop(fullText.length() - fullText.lastIndexOf(AMPrefixSuffix::separator2()));

		return fullText;
	}

	void setText(const QString & text) {
		QLineEdit::setText(prefix_ + AMPrefixSuffix::separator1() + text + AMPrefixSuffix::separator2() + suffix_);
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
