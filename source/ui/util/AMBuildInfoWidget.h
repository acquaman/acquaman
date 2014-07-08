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
#ifndef AMBUILDINFOWIDGET_H
#define AMBUILDINFOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <util/AMBuildInfo.h>
/**
 * @brief Widget to display Acquaman build information
 */
class AMBuildInfoWidget : public QWidget
{
	Q_OBJECT
private:
	/// @brief Label for displaying the commit hash
	QLabel* commitSHALabel_;
	/// @brief Label for displaying the branch name
	QLabel* branchNameLabel_;
	/// @brief Label for displaying the tag name
	QLabel* tagNameLabel_;
	/// @brief Label for displaying a description
	QLabel* describeNameLabel_;
	/// @brief Label for displaying the last author to make a commit
	QLabel* lastCommitAuthorLabel_;
	/// @brief Label for displaying the date of the last commit
	QLabel* commitDateLabel_;
public:
	/// @brief Creates an instance of an AMBuildInfoWidget with blank information. Use setBuildInfo() to
	/// display information for a specific build
	explicit AMBuildInfoWidget(QWidget *parent = 0);
	/// @brief Sets the widget to display information for the provided build
	void setBuildInfo(const AMBuildInfo* info);
signals:
	
public slots:
	
};

#endif // AMBUILDINFOWIDGET_H
