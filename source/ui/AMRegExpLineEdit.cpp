#include "AMRegExpLineEdit.h"
#include <QToolTip>

AMRegExpLineEdit::AMRegExpLineEdit(const QString &pattern,Qt::CaseSensitivity caseSensitivity, const QString &warningMessage,  QWidget *parent) :
	QLineEdit(parent)
{
	warningMessage_ = warningMessage;
	validIfMatches_ = true;
	regExp_.setPattern(pattern);
	regExp_.setCaseSensitivity(caseSensitivity);
	connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
	validate();
}

void AMRegExpLineEdit::setValidIfMatches(bool validIfMatches)
{
	validIfMatches_ = validIfMatches;
	validate();
}

bool AMRegExpLineEdit::isValid()
{
	return isValid_;
}

void AMRegExpLineEdit::validate()
{
	if(regExp_.isEmpty())
		return;

	bool valid = false;
	if(validIfMatches_)
		valid = (regExp_.indexIn(text()) != -1);
	else
		valid = (regExp_.indexIn(text()) == -1);


	if(!valid)
	{
		if(isValid_)
		{
			isValid_ = false;
			if(!warningMessage_.isEmpty())
			{
				QToolTip::showText(this->mapToGlobal(QPoint(this->width(), 0)), warningMessage_);
			}
			QPalette curPal = palette();
			curPal.setColor(QPalette::Text, Qt::red);
			setPalette(curPal);
			update();
			emit validStateChanged(isValid_);
		}
	}
	else
	{
		if(!isValid_)
		{
			QToolTip::hideText();
			isValid_ = true;
			QPalette curPal = palette();
			curPal.setColor(QPalette::Text, Qt::black);
			setPalette(curPal);
			update();
			emit validStateChanged(isValid_);
		}
	}
}

void AMRegExpLineEdit::setPattern(const QString &pattern)
{
	regExp_.setPattern(pattern);
	validate();
}

void AMRegExpLineEdit::onTextChanged(const QString&)
{
	validate();
}
