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


#include "AMControlInfoListView.h"

#include <QLineEdit>
#include <QBoxLayout>

 AMControlInfoListView::~AMControlInfoListView(){}
AMControlInfoListView::AMControlInfoListView(const AMControlInfoList &infoList, QMap<int, QString> enumOverrides, QWidget *parent) :
	QGroupBox(parent)
{
	setTitle(infoList.name());

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->setContentsMargins(2,2,2,2);
	setLayout(mainVL);

	bool overridingEnums = false;
	if(enumOverrides.count() > 0)
		overridingEnums = true;

	QGroupBox *tempGroupBox;
	QLineEdit *tempLineEdit;
	QVBoxLayout *tempVL;
	for(int x = 0, size = infoList.count(); x < size; x++){
		tempGroupBox = new QGroupBox(infoList.at(x).description());
		QString lineEditString;
		if(overridingEnums)
			lineEditString = enumOverrides.value(infoList.at(x).value());
		else
			lineEditString = QString("%1").arg(infoList.at(x).value());
		tempLineEdit = new QLineEdit(lineEditString);
		tempVL = new QVBoxLayout();
		tempVL->addWidget(tempLineEdit);
		tempVL->setContentsMargins(2,2,2,2);
		tempGroupBox->setLayout(tempVL);

		mainVL->addWidget(tempGroupBox);
	}

	setFixedSize(300, 200);
}
