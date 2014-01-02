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


#ifndef AMTOPFRAME_H
#define AMTOPFRAME_H

#include <QFrame>
#include <QIcon>

class QLabel;
class QHBoxLayout;

/// A simple class to provide a nice top frame that all main widgets can use to provide a consistent feel to widgets inside of AM.
class AMTopFrame : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  Provide a title and optionally an icon.
 	virtual ~AMTopFrame();
	AMTopFrame(const QString &title, const QIcon &icon = QIcon(":/system-software-update.png"), QWidget *parent = 0);

	/// Returns the title of the frame.
	const QString title() const;
	/// Returns the icon of the frame.
	const QIcon icon() const;
	/// Returns the layout of the bar if you wish to customize it.
	QHBoxLayout* frameLayout();

public slots:
	/// Sets the title of the frame.
	void setTitle(const QString &title);
	/// Sets the icon for the frame.
	void setIcon(const QIcon &icon);

protected:
	/// The label that holds the icon pixmap.
	QLabel *iconLabel_;
	/// Holds the icon that's displayed in the label.
	QIcon icon_;
	/// The label that holds the title.
	QLabel *titleLabel_;
	/// The separator that provides the distinction between the content of this widget and the rest of whichever widget it is being the title for.
	QFrame *separator_;
	/// The layout of the frame.
	QHBoxLayout *hl_;
};

#endif // AMTOPFRAME_H
