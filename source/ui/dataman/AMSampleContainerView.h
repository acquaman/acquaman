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


#ifndef AMSAMPLECONTAINERVIEW_H
#define AMSAMPLECONTAINERVIEW_H

/*
#include <QListView>

class AMSampleContainer;
class AMSampleView;

/// This class is a view for a list of AMSamples
/// uses index changed and setCurrentSelection to coordinate
/// selected sample with AMSampleCamera
class AMSampleContainerView : public QListView
{
    Q_OBJECT
public:
    /// Constructors
    AMSampleContainerView(QWidget* parent = 0);
    AMSampleContainerView(AMSampleContainer* sampleContainer, QWidget *parent = 0);

    /// set the sample container to visualize
    void setSampleContainer(AMSampleContainer* sampleContainer);

    /// called when the current index is changed
    void currentChanged(const QModelIndex &current, const QModelIndex &previous);

public slots:
    /// sets the current selection
    void setCurrentSelection(int);

//    void updateSampleName(int, QString);

signals:
    /// signal emitted when the current index has been changed
    void indexChanged(int);

protected:
    /// initialization
    void init();
protected:
    /// the sample container
    AMSampleContainer* sampleContainer_;
    /// the view for individual samples
    AMSampleView* sampleView_;
};
*/

#endif // AMSAMPLECONTAINERVIEW_H
