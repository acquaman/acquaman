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


#ifndef AMSAMPLEBRIEFVIEW_H
#define AMSAMPLEBRIEFVIEW_H

#include <QWidget>

class QLabel;
class QPushButton;

class AMSample;
class AMSampleReadOnlyView;

class AMSampleBriefView : public QWidget
{
Q_OBJECT
public:
	/// Constructor takes a sample to view (or a null pointer)
	AMSampleBriefView(const AMSample *sample = 0, QWidget *parent = 0);

public slots:
	/// Sets the sample
	void setSample(const AMSample *sample);

protected slots:
	/// Handles clicks from the button and launches a new AMSampleView
	void onShowSampleInformationButtonClicked();

protected:
	/// The sample we intend to view
	const AMSample *sample_;

	/// Label that holds the name of the sample
	QLabel *nameLabel_;
	/// Push button for launching the expanded sample view
	QPushButton *showSampleInformationButton_;

	/// A full AMSampleView that is launched (or shown) when the show information button is clicked
	AMSampleReadOnlyView *sampleReadOnlyView_;
};

#endif // AMSAMPLEBRIEFVIEW_H
