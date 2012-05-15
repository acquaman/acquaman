/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSXRFElementView.h"

#include <QGridLayout>
#include <QStringList>
#include <QToolButton>

VESPERSXRFElementView::VESPERSXRFElementView(XRFElement *el, QWidget *parent)
	: QWidget(parent)
{
	element_ = el;
	minimumEnergy_ = 0;
	maximumEnergy_ = 1e6;

	name_ = new QLabel;
	name_->setFont(QFont("Times New Roman", 16));
	number_ = new QLabel;
	symbol_ = new QLabel;
	symbol_->setFont(QFont("Times New Roman", 45, 10));

	LineView *temp;

	for (int i = 0; i < 6; i++){

		temp = new LineView;
		connect(temp, SIGNAL(lineCheckedChanged(bool,QString)), this, SLOT(onLineCheckedChanged(bool,QString)));
		lines_ << temp;
	}

	if (el)
		setElement(el);

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(name_);
	titleLayout->addWidget(symbol_);

	QVBoxLayout *linesLayout = new QVBoxLayout;
	linesLayout->setSpacing(0);
	for (int i = 0; i < lines_.size(); i++)
		linesLayout->addWidget(lines_.at(i), Qt::AlignRight);

	QVBoxLayout *viewLayout = new QVBoxLayout;
	viewLayout->addLayout(titleLayout);
	viewLayout->addLayout(linesLayout);
	viewLayout->addStretch();

	setMaximumSize(300, 300);

	setLayout(viewLayout);
}

void VESPERSXRFElementView::setElement(XRFElement *el)
{
	element_ = el;
	name_->setText(el->name());
	number_->setText(QString::number(el->atomicNumber()));
	symbol_->setText(el->symbol());
	fillEmissionLines();
}


void VESPERSXRFElementView::fillEmissionLines()
{
	QList<QPair<QString, QString> > list(element_->emissionLines());

	int lineIndex = 0;

	if (element_->hasLinesSelected()){

		for (int i = 0; i < list.size(); i++){

			if (list.at(i).first.contains("1") && energyWithinTolerance(list.at(i).second.toDouble())){

				if (element_->linesSelected().contains(list.at(i).first))
					lines_.at(lineIndex++)->setLine(list.at(i), true);
				else
					lines_.at(lineIndex++)->setLine(list.at(i), false);
			}
		}
	}
	else{

		for (int i = 0; i < list.size(); i++){

			if (list.at(i).first.contains("1") && energyWithinTolerance(list.at(i).second.toDouble()))
				lines_.at(lineIndex++)->setLine(list.at(i), false);
		}
	}

	for ( ; lineIndex < lines_.size(); lineIndex++)
		lines_.at(lineIndex)->setLine(qMakePair(QString(), QString()));
}

void VESPERSXRFElementView::onLineCheckedChanged(bool checked, QString lineName)
{
	if (checked)
		emit addLine(lineName);
	else
		emit removeLine(lineName);
}
