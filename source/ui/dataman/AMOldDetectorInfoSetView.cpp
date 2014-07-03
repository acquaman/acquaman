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


#include "AMOldDetectorInfoSetView.h"

#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>

 AMOldDetectorInfoSetView::~AMOldDetectorInfoSetView(){}
AMOldDetectorInfoSetView::AMOldDetectorInfoSetView(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QGroupBox(parent)
{
	internalView_ = new AMOldDetectorInfoSetViewInternal(infoSet, parent);

	setTitle(infoSet.name());
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(internalView_);
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(scrollArea_);
	mainVL->setContentsMargins(0,0,0,0);
	setLayout(mainVL);
}

 AMOldDetectorInfoSetViewInternal::~AMOldDetectorInfoSetViewInternal(){}
AMOldDetectorInfoSetViewInternal::AMOldDetectorInfoSetViewInternal(const AMOldDetectorInfoSet &infoSet, QWidget *parent) :
	QWidget(parent)
{
	QVBoxLayout *mainVL = new QVBoxLayout();

	QLabel *tempLabel;
	QCheckBox *tempCheckBox;
	QHBoxLayout *tempHL;
	for(int x = 0, size = infoSet.count(); x < size; x++){
		tempLabel = new QLabel(infoSet.detectorInfoAt(x)->description());
		tempCheckBox = new QCheckBox();
		tempCheckBox->setChecked(infoSet.isActiveAt(x));
		tempHL = new QHBoxLayout();
		tempHL->addWidget(tempLabel);
		tempHL->addWidget(tempCheckBox);
		mainVL->addLayout(tempHL);
	}

	setLayout(mainVL);
}
