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


#include "IDEASScanConfigurationViewHolder3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMScanAction.h"

IDEASScanConfigurationViewHolder3::~IDEASScanConfigurationViewHolder3(){}

IDEASScanConfigurationViewHolder3::IDEASScanConfigurationViewHolder3(IDEASXASScanConfigurationView *view, QWidget *parent)
	: AMScanConfigurationViewHolder3(view, parent)
{
}

AMAction3 *IDEASScanConfigurationViewHolder3::createAction()
{
	if (view_){

		const IDEASXASScanConfiguration *config = qobject_cast<const IDEASXASScanConfiguration *>(view_->configuration());

		if (config){

			AMLoopAction3 *loop = new AMLoopAction3(new AMLoopActionInfo3(config->numberOfScans(), config->name(), config->description()));
			loop->addSubAction(new AMScanAction(new AMScanActionInfo(config->createCopy())));
			return loop;
		}
	}

	return 0;
}
