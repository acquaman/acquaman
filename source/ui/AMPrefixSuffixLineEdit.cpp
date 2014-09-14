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


#include "AMPrefixSuffixLineEdit.h"

// Fortunately, this function gets called every single time the line edit is typed into. We use it to intercept and negate any editing of the prefix or suffix.
AMPrefixSuffixValidator::State AMPrefixSuffixValidator::validate ( QString & input, int & pos ) const {
	// possible situations:
	/*	- normal editing inside the allowed "actual" text area
		- just backspaced over the prefix separator
		- just deleted the suffix separator
		- clicked over into the prefix or suffix regions and typed some text that shouldn't be there. (We want to capture this character and put it back where it belongs)
		- setText editing that gave a correct string, but with the pos at the very end.
		   */


	// Did they just delete the prefix separator? (ie: by hitting backspace) If so, erase everything prior to their position, and add it back
	if(!input.contains(AMPrefixSuffix::separator1())) {
		input.remove(0, pos);
		input.prepend(AMPrefixSuffix::separator1());
		input.prepend(prefix_);
		pos = prefix_.length()+1;
	}

	// So we still have the separator.  What's before it might be bad or good.
	else {
		int indexOfSep = input.indexOf(AMPrefixSuffix::separator1());

		// all good?
		if(input.left(indexOfSep) == prefix_) {
			// do nothing...
		}

		else {

			// Did they add or replace a char in the prefix? In this case, the pos will be before the separator, and the number of chars before the separator will be the same or longer than it should be.
			if( pos > 0 && pos <= indexOfSep && indexOfSep >= prefix_.length() ){
				QChar rescued = input.at(pos - 1);
				input.remove(0, indexOfSep + 1);
				input.prepend(rescued);
				pos = prefix_.length() + 2;
			}
			// any other damage (or even non-damage, for now) we handle by deleting everything up to and including the separator, and adding it all back.
			else {
				input.remove(0, indexOfSep + 1);
			}
			// add the prefix back...
			input.prepend(AMPrefixSuffix::separator1());
			input.prepend(prefix_);
		}
	}

	// Did they just delete the suffix sep?
	if(!input.contains(AMPrefixSuffix::separator2())) {
		input.chop(input.length() - pos);
		input.append(AMPrefixSuffix::separator2());
		input.append(suffix_);
	}
	// otherwise, we still have the separator
	else {
		int indexOfSep = input.lastIndexOf(AMPrefixSuffix::separator2());

		// all good?
		if(input.mid(indexOfSep+1) == suffix_) {
			// do nothing
		}
		else {
			// did they add or delete a character in the suffix?
			if(pos > indexOfSep && input.length() - indexOfSep >= suffix_.length()) {
				QChar rescued = input.at(pos-1);
				input.chop(input.length() - indexOfSep);
				input.append(rescued);
			}
			// otherwise... (any other kind of damage, handle generically)
			else {
				input.chop(input.length() - indexOfSep);
			}

			input.append(AMPrefixSuffix::separator2());
			input.append(suffix_);
		}
	}


	// make sure they keep their position within where it's allowed to be...
	if(pos < prefix_.length() + 1 )
		pos = prefix_.length() + 1;

	if( pos > input.length() - suffix_.length() - 1)
		pos = input.length() - suffix_.length() - 1;

	return QValidator::Acceptable;
}

 AMPrefixSuffixLineEdit::~AMPrefixSuffixLineEdit(){}
AMPrefixSuffixLineEdit::AMPrefixSuffixLineEdit(const QString& prefix, const QString& suffix, QWidget *parent) :
		QLineEdit(parent)
{
	setPrefix(prefix);
	setSuffix(suffix);
	setValidator(&validator_);
}


void AMPrefixSuffixLineEdit::setPrefix(const QString& prefix)
{
	// clear the old prefix
	QString originalText = QLineEdit::text();
	if(originalText.contains(AMPrefixSuffix::separator1()))
		originalText.remove(0, originalText.indexOf(AMPrefixSuffix::separator1()) + 1);

	// ad the new one
	originalText.prepend(AMPrefixSuffix::separator1());
	originalText.prepend(prefix_ = prefix);
	QLineEdit::setText(originalText);

	validator_.setPrefix(prefix_);

}
void AMPrefixSuffixLineEdit::setSuffix(const QString& suffix)
{
	QString oldText = QLineEdit::text();

	// clear the old suffix
	if(oldText.contains(AMPrefixSuffix::separator2()))
		oldText.chop(oldText.length() - oldText.lastIndexOf(AMPrefixSuffix::separator2()));

	oldText.append(AMPrefixSuffix::separator2());
	oldText.append(suffix_ = suffix);
	QLineEdit::setText(oldText);

	validator_.setSuffix(suffix_);
}
