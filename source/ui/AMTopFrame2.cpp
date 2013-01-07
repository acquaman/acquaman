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


#include "AMTopFrame2.h"
#include "ui_AMTopFrame2.h"

#include "util/AMFontSizes.h"

AMTopFrame2::AMTopFrame2(const QString& title, const QIcon& icon, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AMTopFrame2)
{
    ui->setupUi(this);
	ui->title->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	ui->statusTextLeft->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	ui->statusTextRight->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setFixedHeight(90);

	setTitle(title);
	setIcon(icon);
	ui->statusTextLeft->setText(QString());
	ui->statusTextRight->setText(QString());
}

AMTopFrame2::~AMTopFrame2()
{
	delete ui;
}

QString AMTopFrame2::title() const
{
	return ui->title->text();
}

QString AMTopFrame2::leftSubText() const
{
	return ui->statusTextLeft->text();
}

QString AMTopFrame2::rightSubText() const
{
	return ui->statusTextRight->text();
}


void AMTopFrame2::setTitle(const QString &title)
{
	ui->title->setText(title);
}

void AMTopFrame2::setIcon(const QIcon& icon)
{
	ui->icon->setPixmap(icon.pixmap(36, 36));
}

void AMTopFrame2::setLeftSubText(const QString &subText)
{
	ui->statusTextLeft->setText(subText);
}

void AMTopFrame2::setRightSubText(const QString &subText)
{
	ui->statusTextRight->setText(subText);
}
