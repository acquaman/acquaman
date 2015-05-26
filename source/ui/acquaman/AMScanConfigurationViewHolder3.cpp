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

#include "AMScanConfigurationViewHolder3.h"

#include <QSpinBox>
#include <QLabel>

#include "acquaman/AMScanConfiguration.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMScanAction.h"
#include "ui/acquaman/AMScanConfigurationView.h"

 AMScanConfigurationViewHolder3::~AMScanConfigurationViewHolder3(){}
AMScanConfigurationViewHolder3::AMScanConfigurationViewHolder3(AMScanConfigurationView* view, bool enableLoopAction, QWidget *parent) :
	AMActionRunnerAddActionBar3("Scan", enableLoopAction, parent)
{
	view_ = view;

	if(view_)
		addWidget(view_);

	disabledWarning_ = new QLabel("");
	QFont warningsFont;
	warningsFont.setPointSize(32);
	disabledWarning_->setFont(warningsFont);
	disabledWarning_->setStyleSheet( "QLabel{ color: red }" );
	addWidget(disabledWarning_);


}

void AMScanConfigurationViewHolder3::setView(AMScanConfigurationView *view) {
	// delete old view, if it exists
	if(view_)
		view_->deleteLater();

	view_ = view;
	if(view_) {
		addWidget(view_);
	}
}

void AMScanConfigurationViewHolder3::setEnabled(bool enabled){
	AMActionRunnerAddActionBar3::setEnabled(enabled);

	if(view_ && enabled)
		disabledWarning_->setText("");
	else if(view_ && !enabled)
		disabledWarning_->setText("This Configuration Is Disabled");
}

AMAction3 * AMScanConfigurationViewHolder3::createScan()
{
	if(view_)
		return new AMScanAction(new AMScanActionInfo(view_->configuration()->createCopy()));

	return 0;
}

AMAction3* AMScanConfigurationViewHolder3::createMultipleScans()
{
	if (view_){

		const AMScanConfiguration *config = view_->configuration();
		if (config) {
			int iteration = iterationsBox_->value();
			if (iteration > 1) {
				AMLoopAction3 *loopAction = new AMLoopAction3(new AMLoopActionInfo3(iterationsBox_->value(), config->name(), config->description()));
				loopAction->addSubAction(createScan());
				return loopAction;
			} else {
				return createScan();
			}
		}
	}

	return 0;
}
