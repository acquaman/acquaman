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
#include "AMAboutDialog.h"

#include "AMBuildInfoWidget.h"
#include "util/AMBuildReporter.h"
AMAboutDialog::AMAboutDialog(QWidget *parent) :
	QDialog(parent)
{

	setWindowTitle("About Acquaman");

	QLabel* titleLabel = new QLabel("Acquaman");
	QFont font = titleLabel->font();
	font.setPointSize(14);
	titleLabel->setFont(font);

	QLabel* descriptionLabel = new QLabel("ACQUisition and data MANagement");

	QLabel* legalLabel = new QLabel("Acquaman is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.");

	buildInfoWidget_ = new AMBuildInfoWidget();
	buildInfoWidget_->setBuildInfo(AMBuildReporter::buildReporter()->buildInfo());

	QGridLayout* mainLayout = new QGridLayout();

	mainLayout->addWidget(titleLabel, 0, 0, 1, 2);
	mainLayout->addWidget(descriptionLabel, 1,0,1,2);

	mainLayout->addWidget(buildInfoWidget_, 2, 0, 1, 2, Qt::AlignCenter);
	mainLayout->addWidget(legalLabel,3, 0, 1, 2);
	setLayout(mainLayout);
}
