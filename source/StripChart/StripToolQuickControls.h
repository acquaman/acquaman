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


#ifndef STRIPTOOLQUICKCONTROLS_H
#define STRIPTOOLQUICKCONTROLS_H

#include <QWidget>
#include <QtGui>

#include "beamline/AMPVControl.h"
#include "StripChart/StripToolModel.h"
#include "StripChart/StripToolListView.h"

/// This class is a sort of optional sidebar that provides a number of ways for the user to interact with an added pv, as well as a quick way to add a new one.
class StripToolQuickControls : public QWidget
{
    Q_OBJECT

public:
    explicit StripToolQuickControls(QWidget *parent = 0);
    virtual ~StripToolQuickControls();
    
signals:
    void reset();
    void clearName();
    void clearMessage();
    void nameEntryEnabled(bool);
    void nameEntryFocus();
    void buttonEnabled(bool);
    void addPV(const QString &pvName);
    void error(const QString &errorMessage);
    void pausePVs();
    void resumePVs();

protected:
    StripToolModel *model_;
    StripToolListView *listView_;
    QLineEdit *pvNameLineEdit_;
    QPushButton *addButton_;
    QPushButton *pauseButton_;
    QPushButton *resumeButton_;
    QLabel *message_;

public:
    void setModel(StripToolModel *newModel);
    
protected slots:
    void resetControls();
    void displayMessage(const QString &text);
    /// When the add button is clicked, information entry is disabled while we test to see if the entered pv name is valid.
    void addClicked();
    void pauseClicked();
    void resumeClicked();
};

#endif // STRIPTOOLQUICKCONTROLS_H
