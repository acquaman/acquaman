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


#include "AMBeamlineCameraWidgetWithSourceTabs.h"

#include <QTabBar>
#include <QVBoxLayout>

 AMBeamlineCameraWidgetWithSourceTabs::~AMBeamlineCameraWidgetWithSourceTabs(){}
AMBeamlineCameraWidgetWithSourceTabs::AMBeamlineCameraWidgetWithSourceTabs(const QUrl& initialSource, const QString& initialDescription, QWidget *parent, bool useOpenGl) :
	AMBeamlineCameraWidget(parent, useOpenGl)
{
	// Add GUI element: the tab bar:
	tabBar_ = new QTabBar();
	tabBar_->setDocumentMode(false);
	QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(layout());
	vl->insertWidget(1, tabBar_, 0);

	// Make connections
	connect(tabBar_, SIGNAL(currentChanged(int)), this, SLOT(playSourceAt(int)));
	// Not used for now: Should users be able to close tabs intentionally?
		// connect(tabBar_, SIGNAL(tabCloseRequested(int)), this, SLOT(removeSource(int));

	// if we have an initial source, add it.
	if(initialSource.isValid()) {
		addSource(initialDescription, initialSource);
	}
}

void AMBeamlineCameraWidgetWithSourceTabs::addSource(const QString &description, const QUrl &sourceUrl)
{
	sourceUrls_ << sourceUrl;
	tabBar_->addTab(description);
}

void AMBeamlineCameraWidgetWithSourceTabs::removeSource(int index)
{
	if(index<0 || index>=sourceCount())
		return;
	sourceUrls_.removeAt(index);
	tabBar_->removeTab(index);
}
