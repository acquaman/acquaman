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


#ifndef AMBUILDINFO_H
#define AMBUILDINFO_H

#include <QString>

class AMBuildInfo
{

public:
	AMBuildInfo();

	QString commitSHA() const;
	QString branchName() const;
	QString tagName() const;
	QString describeName() const;
	QString lastCommitAuthor() const;
	QString commitDate() const;

protected:
	void setCommitSHA(const QString &commitSHA);
	void setBranchName(const QString &branchName);
	void setTagName(const QString &tagName);
	void setDescribeName(const QString &describeName);
	void setLastCommitAuthor(const QString &lastCommitAuthor);
	void setCommitDate(const QString &commitDate);

protected:
	QString commitSHA_;
	QString branchName_;
	QString tagName_;
	QString describeName_;
	QString lastCommitAuthor_;
	QString commitDate_;
};

#endif // AMBUILDINFO_H
