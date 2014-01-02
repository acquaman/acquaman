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

#include "AMScanConfigurationViewHolder3.h"

#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QButtonGroup>

#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "actions3/actions/AMScanAction.h"

 AMScanConfigurationViewHolder3::~AMScanConfigurationViewHolder3(){}
AMScanConfigurationViewHolder3::AMScanConfigurationViewHolder3(AMScanConfigurationView* view, QWidget *parent) :
	AMActionRunnerAddActionBar3("Scan", parent)
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
		delete view_;

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

AMAction3 * AMScanConfigurationViewHolder3::createAction()
{
	if(view_)
		return new AMScanAction(new AMScanActionInfo(view_->configuration()->createCopy()));

	return 0;
}
