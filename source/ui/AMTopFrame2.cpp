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


#include "AMTopFrame2.h"

#include "util/AMFontSizes.h"

AMTopFrame2::AMTopFrame2(const QString& title, const QIcon& icon, QWidget *parent) :
	QWidget(parent)
{
	setupUi();
	title_->setStyleSheet("font: " AM_FONT_LARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	statusTextLeft_->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	statusTextRight_->setStyleSheet("color: white;\nfont: bold " AM_FONT_SMALL_ "pt \"Lucida Grande\"");
	setFixedHeight(90);

	setTitle(title);
	setIcon(icon);
	statusTextLeft_->setText(QString());
	statusTextRight_->setText(QString());
}

AMTopFrame2::~AMTopFrame2()
{
}

QString AMTopFrame2::title() const
{
	return title_->text();
}

QString AMTopFrame2::leftSubText() const
{
	return statusTextLeft_->text();
}

QString AMTopFrame2::rightSubText() const
{
	return statusTextRight_->text();
}


void AMTopFrame2::setTitle(const QString &title)
{
	title_->setText(title);
}

void AMTopFrame2::setIcon(const QIcon& icon)
{
	icon_->setPixmap(icon.pixmap(36, 36));
}

void AMTopFrame2::setLeftSubText(const QString &subText)
{
	statusTextLeft_->setText(subText);
}

void AMTopFrame2::setRightSubText(const QString &subText)
{
	statusTextRight_->setText(subText);
}

void AMTopFrame2::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMTopFrame2"));
	resize(1116, 90);

	verticalLayout_ = new QVBoxLayout();
	verticalLayout_->setSpacing(0);
	verticalLayout_->setContentsMargins(0, 0, 0, 0);
	verticalLayout_->setObjectName(QString::fromUtf8("verticalLayout"));

	titleFrame_ = new QFrame();
	titleFrame_->setObjectName(QString::fromUtf8("titleFrame"));
	titleFrame_->setMinimumSize(QSize(0, 60));
	titleFrame_->setStyleSheet(QString::fromUtf8("background-color: rgb(236, 236, 236);"));
	horizontalLayout3_ = new QHBoxLayout(titleFrame_);
	horizontalLayout3_->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	titleFrame_->setLayout(horizontalLayout3_);

	icon_ = new QLabel();
	icon_->setObjectName(QString::fromUtf8("icon"));
	icon_->setMaximumSize(QSize(36, 36));
	icon_->setPixmap(QPixmap(QString::fromUtf8(":/system-software-update.png")));
	icon_->setScaledContents(true);

	horizontalLayout3_->addWidget(icon_);

	title_ = new QLabel();
	title_->setObjectName(QString::fromUtf8("title"));
	title_->setStyleSheet(QString::fromUtf8("font: 16pt \"Lucida Grande\";\n"
"color: rgb(79, 79, 79);"));

	horizontalLayout3_->addWidget(title_);

	horizontalSpacer_ = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout3_->addItem(horizontalSpacer_);


	verticalLayout_->addWidget(titleFrame_);

	statusFrame_ = new QFrame();
	statusFrame_->setObjectName(QString::fromUtf8("statusFrame"));
	statusFrame_->setMinimumSize(QSize(0, 30));
	statusFrame_->setMaximumSize(QSize(16777215, 30));
	statusFrame_->setStyleSheet(QString::fromUtf8("QFrame#statusFrame {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));\n"
"border-bottom: 1px solid black;\n"
"}"));
	horizontalLayout4_ = new QHBoxLayout();
	horizontalLayout4_->setObjectName(QString::fromUtf8("horizontalLayout_4"));
	horizontalLayout4_->setContentsMargins(-1, 6, -1, 6);
	statusFrame_->setLayout(horizontalLayout4_);

	statusTextLeft_ = new QLabel();
	statusTextLeft_->setObjectName(QString::fromUtf8("statusTextLeft"));
	statusTextLeft_->setStyleSheet(QString::fromUtf8("color: white;\n"
"font: bold 10pt \"Lucida Grande\""));

	horizontalLayout4_->addWidget(statusTextLeft_);

	horizontalSpacer2_ = new QSpacerItem(478, 9, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout4_->addItem(horizontalSpacer2_);

	statusTextRight_ = new QLabel();
	statusTextRight_->setObjectName(QString::fromUtf8("statusTextRight"));
	statusTextRight_->setStyleSheet(QString::fromUtf8("color: white;\n"
"font: bold 10pt \"Lucida Grande\""));

	horizontalLayout4_->addWidget(statusTextRight_);


	verticalLayout_->addWidget(statusFrame_);

	titleFrame_->raise();
	statusFrame_->raise();

	setLayout(verticalLayout_);

	setWindowTitle(QApplication::translate("AMTopFrame2", "Form", 0, QApplication::UnicodeUTF8));
			icon_->setText(QString());
			title_->setText(QApplication::translate("AMTopFrame2", "Title", 0, QApplication::UnicodeUTF8));
			statusTextLeft_->setText(QApplication::translate("AMTopFrame2", "status text left", 0, QApplication::UnicodeUTF8));
			statusTextRight_->setText(QApplication::translate("AMTopFrame2", "status text right", 0, QApplication::UnicodeUTF8));
}
