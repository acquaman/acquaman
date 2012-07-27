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


#include "REIXSScanConfigurationViewHolder.h"

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
#include "actions2/actions/REIXS/REIXSXESScanAction.h"
#include "actions2/actions/REIXS/REIXSXASScanAction.h"

REIXSScanConfigurationViewHolder::REIXSScanConfigurationViewHolder(AMScanConfigurationView *view, QWidget *parent)
	: AMActionRunnerAddActionBar("Scan", parent)
{
	view_ = view;
	addWidget(view);
}


AMAction * REIXSScanConfigurationViewHolder::createAction()
{
	const REIXSXESScanConfiguration* xesConfig = qobject_cast<const REIXSXESScanConfiguration*>(view_->configuration());

	if(xesConfig)
		return new REIXSXESScanAction(qobject_cast<REIXSXESScanConfiguration*>(xesConfig->createCopy()));

	const REIXSXASScanConfiguration* xasConfig = qobject_cast<const REIXSXASScanConfiguration*>(view_->configuration());
	if(xasConfig)
		return new REIXSXASScanAction(qobject_cast<REIXSXASScanConfiguration*>(xasConfig->createCopy()));

	return 0;
}

