#include "AMPrefixSuffixLineEdit.h"

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

