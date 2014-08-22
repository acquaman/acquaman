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
#include "AMBuildInfoWidget.h"

AMBuildInfoWidget::AMBuildInfoWidget(QWidget *parent) :
	QWidget(parent)
{
	commitSHALabel_ = new QLabel();
	branchNameLabel_ = new QLabel();
	tagNameLabel_ = new QLabel();
	describeNameLabel_ = new QLabel();
	lastCommitAuthorLabel_ = new QLabel();
	commitDateLabel_ = new QLabel();

	QFormLayout* mainLayout = new QFormLayout();

	mainLayout->addRow("Hash:", commitSHALabel_);
	mainLayout->addRow("Branch:", branchNameLabel_);
	mainLayout->addRow("Tag:", tagNameLabel_);
	mainLayout->addRow("Description:", describeNameLabel_);
	mainLayout->addRow("Last Commit Author:", lastCommitAuthorLabel_);
	mainLayout->addRow("Last Commit Date:", commitDateLabel_);

	setLayout(mainLayout);
}

void AMBuildInfoWidget::setBuildInfo(const AMBuildInfo *info)
{
	if(!info)
		return;

	commitSHALabel_->setText(info->commitSHA());
	branchNameLabel_->setText(info->branchName());
	tagNameLabel_->setText(info->tagName());
	describeNameLabel_->setText(info->describeName());
	lastCommitAuthorLabel_->setText(info->lastCommitAuthor());
	commitDateLabel_->setText(info->commitDate());
}
