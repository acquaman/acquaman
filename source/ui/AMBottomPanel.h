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


#ifndef AMBOTTOMPANEL_H
#define AMBOTTOMPANEL_H

#include <QFrame>
#include <QHBoxLayout>

/// This is a base class for the bottom widget of the AM main window.  It has no features except for defining the standard stylesheet and a way to change it.
class AMBottomPanel : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  Builds the widget in the current standard style.
 	virtual ~AMBottomPanel();
	AMBottomPanel(QWidget *parent = 0);

protected:
	/// Returns the layout of the panel.
	QHBoxLayout *panelLayout() const { return layout_; }

	/// The pointer to the layout of the panel.
	QHBoxLayout *layout_;
};

#endif // AMBOTTOMPANEL_H
