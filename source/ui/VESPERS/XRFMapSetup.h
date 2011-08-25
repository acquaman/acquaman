/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef XRFMAPSETUP_H
#define XRFMAPSETUP_H

#include <QWidget>
#include <QRadioButton>
#include <QCheckBox>

/*!
	This widget sets up the arguments to boot up the NDMapper software without users having to go through the terminal application.
  */
class XRFMapSetup : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds the widget.
	explicit XRFMapSetup(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles launching the program with the given arguments.
	void launchNDMapper();

protected:
	// Member variables.
	QRadioButton *dim2D_;
	QRadioButton *dim3D_;

	QRadioButton *vortex1El_;
	QRadioButton *vortex4El_;

	QCheckBox *ccd_;
	QCheckBox *multipleImages_;

};

#endif // XRFMAPSETUP_H
