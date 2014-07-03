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
