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


#include "AMSampleContainerView.h"

/*
#include "dataman/AMSampleContainer.h"
#include "ui/dataman/AMSampleView.h"
#include "dataman/AMSample.h"
#include <QDebug>
AMSampleContainerView::AMSampleContainerView(QWidget *parent)
    :QListView(parent)
{
    init();
}

AMSampleContainerView::AMSampleContainerView(AMSampleContainer *sampleContainer, QWidget *parent)
    :QListView(parent)
{
    init();
    setSampleContainer(sampleContainer);
}

void AMSampleContainerView::setSampleContainer(AMSampleContainer *sampleContainer)
{
    sampleContainer_ = sampleContainer;
    setModel(sampleContainer_);
}

void AMSampleContainerView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    sampleContainer_->setIndex(current.row());
    int index = sampleContainer_->currentIndex();
    sampleView_->setSample(sampleContainer_->sample(sampleContainer_->currentIndex()));
    sampleView_->show();

    emit indexChanged(index);

    update(QModelIndex());
}

void AMSampleContainerView::setCurrentSelection(int rowIndex)
{
    QModelIndex modelIndex = sampleContainer_->index(rowIndex);
    setCurrentIndex(modelIndex);
}

void AMSampleContainerView::init()
{
    setWindowTitle("Sample Container");
    sampleView_ = new AMSampleView();
}

*/
