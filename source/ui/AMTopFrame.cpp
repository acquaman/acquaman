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


#include "AMTopFrame.h"

#include <QLabel>
#include <QHBoxLayout>

#include "util/AMFontSizes.h"

 AMTopFrame::~AMTopFrame(){}
AMTopFrame::AMTopFrame(const QString &title, const QIcon &icon, QWidget *parent)
	: QFrame(parent)
{
	icon_ = icon;
	iconLabel_ = new QLabel();
	iconLabel_->setPixmap(icon_.pixmap(36,36));
	iconLabel_->setFixedSize(36, 36);
	iconLabel_->setScaledContents(true);

	titleLabel_ = new QLabel(title);
	QString titleStyle = "QLabel{ font: " AM_FONT_LARGE_ "pt \"Lucida Grande\"; color: rgb(79,79,79)}";
	titleLabel_->setStyleSheet(titleStyle);
	titleLabel_->setFixedHeight(36);

	separator_ = new QFrame();
	separator_->setFrameStyle(QFrame::HLine|QFrame::Sunken);
	separator_->setLineWidth(0);
	separator_->setMidLineWidth(1);

	hl_ = new QHBoxLayout();
	hl_->addWidget(iconLabel_);
	hl_->addSpacing(10);
	hl_->addWidget(titleLabel_);
	hl_->addStretch(10);
	hl_->setContentsMargins(0,0,0,0);

	QHBoxLayout *hl2 = new QHBoxLayout();
	hl2->addSpacing(40);
	hl2->addWidget(separator_);
	hl2->addSpacing(30);
	hl2->setContentsMargins(0,0,0,0);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addLayout(hl_);
	vl->addLayout(hl2);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(1);

	setLayout(vl);
	setMinimumSize(0, 60);
	setFixedHeight(60);
}

QHBoxLayout* AMTopFrame::frameLayout(){
	return hl_;
}

void AMTopFrame::setTitle(const QString &title){
	titleLabel_->setText(title);
}

void AMTopFrame::setIcon(const QIcon &icon){
	iconLabel_->setPixmap(icon.pixmap(36, 36));
}

const QString AMTopFrame::title() const
{
	return titleLabel_->text();
}

const QIcon AMTopFrame::icon() const
{
	return icon_;
}
