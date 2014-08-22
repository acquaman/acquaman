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


#ifndef AMSAMPLEEDITOR_H
#define AMSAMPLEEDITOR_H

#include <QWidget>

class QLabel;
class QPushButton;

class AMSample;
class AMSamplePre2013;
class AMDatabase;

class AMSampleEditor : public QWidget
{
Q_OBJECT
public:
	AMSampleEditor(AMDatabase *database, QWidget *parent = 0);
	virtual ~AMSampleEditor();

	void setSample(const AMSample *sample);
	void setSamplePre2013(const AMSamplePre2013 *samplePre2013);

protected slots:
	void onChangeSampleButtonClicked();

protected:
	AMDatabase *database_;
	const AMSample *currentSample_;
	const AMSamplePre2013 *currentSamplePre2013_;

	QLabel *nameLabel_;
	QLabel *createdLabel_;
	QPushButton *changeSampleButton_;
};

#endif // AMSAMPLEEDITOR_H
