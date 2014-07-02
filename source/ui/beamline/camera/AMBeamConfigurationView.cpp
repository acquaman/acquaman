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


#include "AMBeamConfigurationView.h"

#include <QBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QVector>
#include <QVector3D>

#include "beamline/camera/AMBeamConfiguration.h"
#include "dataman/database/AMDbObjectSupport.h"

#include "util/AMErrorMonitor.h"


AMBeamConfigurationView::AMBeamConfigurationView(AMBeamConfiguration *beam, QWidget *parent) :
	QWidget(parent)
{

	if(beam != 0)
		beamModel_ = beam;
	else beamModel_ = new AMBeamConfiguration();

	setWindowTitle("Beam configuration");

	// GUI SETUP
	QVBoxLayout *vbl = new QVBoxLayout();
	vbl->setSpacing(0);
	vbl->setContentsMargins(0,0,0,0);

	QFrame* optionFrame = new QFrame();
	QHBoxLayout *ofl = new QHBoxLayout();
	ofl->setContentsMargins(12,4,12,4);
	ofl->addWidget(new QLabel("Divergent"));
	ofl->addWidget(divergentBox_ = new QCheckBox());
	ofl->addStretch();
	optionFrame->setLayout(ofl);

	QFrame* oneFrame = new QFrame();
	QHBoxLayout *ohl = new QHBoxLayout();
	ohl->setContentsMargins(12,4,12,4);

	QFrame* twoFrame = new QFrame();
	QHBoxLayout *thl = new QHBoxLayout();
	thl->setContentsMargins(12,4,12,4);

	QFrame* oneOneFrame = new QFrame();
	QVBoxLayout *oovl = new QVBoxLayout();
	oovl->setContentsMargins(12,4,12,4);

	QFrame* oneTwoFrame = new QFrame();
	QVBoxLayout *otvl = new QVBoxLayout();
	otvl->setContentsMargins(12,4,12,4);

	QFrame* oneButtonFrame = new QFrame();
	QVBoxLayout *obvl = new QVBoxLayout();
	obvl->setContentsMargins(12,4,12,4);
	obvl->addStretch();
	obvl->addWidget(oneSelect_ = new QPushButton("Select Shape"));
	obvl->addStretch();
	oneButtonFrame->setLayout(obvl);

	QFrame* twoOneFrame = new QFrame();
	QVBoxLayout *tovl = new QVBoxLayout();
	tovl->setContentsMargins(12,4,12,4);

	QFrame* twoTwoFrame = new QFrame();
	QVBoxLayout *ttvl = new QVBoxLayout();
	ttvl->setContentsMargins(12,4,12,4);


	QFrame* twoButtonFrame = new QFrame();
	QVBoxLayout *tbvl = new QVBoxLayout();
	tbvl->setContentsMargins(12,4,12,4);
	tbvl->addStretch();
	tbvl->addWidget(twoSelect_ = new QPushButton("Select Point "));
	tbvl->addStretch();
	twoButtonFrame->setLayout(tbvl);

	QFrame* oneOneDataFrame = new QFrame();
	QHBoxLayout *oodhl = new QHBoxLayout();
	oodhl->setContentsMargins(12,4,12,4);
	oodhl->addWidget(new QLabel("One  "));
	oodhl->addSpacing(20);

	QFrame* oneTwoDataFrame = new QFrame();
	QHBoxLayout *otdhl = new QHBoxLayout();
	otdhl->setContentsMargins(12,4,12,4);
	otdhl->addWidget(new QLabel("Two  "));
	otdhl->addSpacing(20);

	QFrame* oneThreeDataFrame = new QFrame();
	QHBoxLayout *oThdhl = new QHBoxLayout();
	oThdhl->setContentsMargins(12,4,12,4);
	oThdhl->addWidget(new QLabel("Three"));
	oThdhl->addSpacing(20);

	QFrame* oneFourDataFrame = new QFrame();
	QHBoxLayout *ofdhl = new QHBoxLayout();
	ofdhl->setContentsMargins(12,4,12,4);
	ofdhl->addWidget(new QLabel("Four "));
	ofdhl->addSpacing(20);

	QFrame* twoOneDataFrame = new QFrame();
	QHBoxLayout *todhl = new QHBoxLayout();
	todhl->setContentsMargins(12,4,12,4);
	todhl->addWidget(new QLabel("One  "));
	todhl->addSpacing(20);

	QFrame* twoTwoDataFrame = new QFrame();
	QHBoxLayout *ttdhl = new QHBoxLayout();
	ttdhl->setContentsMargins(12,4,12,4);
	ttdhl->addWidget(new QLabel("Two  "));
	ttdhl->addSpacing(20);

	QFrame* twoThreeDataFrame = new QFrame();
	QHBoxLayout *tThdhl = new QHBoxLayout();
	tThdhl->setContentsMargins(12,4,12,4);
	tThdhl->addWidget(new QLabel("Three"));
	tThdhl->addSpacing(20);

	QFrame* twoFourDataFrame = new QFrame();
	QHBoxLayout *tfdhl = new QHBoxLayout();
	tfdhl->setContentsMargins(12,4,12,4);
	tfdhl->addWidget(new QLabel("Four "));
	tfdhl->addSpacing(20);

	QFrame* oneOneLabelsFrame = new QFrame();
	QVBoxLayout *oolvl = new QVBoxLayout();
	oolvl->setContentsMargins(12,4,12,4);
	oolvl->addWidget(oneOneX_ = new QLabel("0"));
	oolvl->addWidget(oneOneY_ = new QLabel("0"));
	oolvl->addWidget(oneOneZ_ = new QLabel("0"));
	oolvl->addStretch();
	oneOneLabelsFrame->setLayout(oolvl);

	QFrame* oneTwoLabelsFrame = new QFrame();
	QVBoxLayout *otlvl = new QVBoxLayout();
	otlvl->setContentsMargins(12,4,12,4);
	otlvl->addWidget(oneTwoX_ = new QLabel("0"));
	otlvl->addWidget(oneTwoY_ = new QLabel("0"));
	otlvl->addWidget(oneTwoZ_ = new QLabel("0"));
	otlvl->addStretch();
	oneTwoLabelsFrame->setLayout(otlvl);

	QFrame* oneThreeLabelsFrame = new QFrame();
	QVBoxLayout *oThlvl = new QVBoxLayout();
	oThlvl->setContentsMargins(12,4,12,4);
	oThlvl->addWidget(oneThreeX_ = new QLabel("0"));
	oThlvl->addWidget(oneThreeY_ = new QLabel("0"));
	oThlvl->addWidget(oneThreeZ_ = new QLabel("0"));
	oThlvl->addStretch();
	oneThreeLabelsFrame->setLayout(oThlvl);


	QFrame* oneFourLabelsFrame = new QFrame();
	QVBoxLayout *oflvl = new QVBoxLayout();
	oflvl->setContentsMargins(12,4,12,4);
	oflvl->addWidget(oneFourX_ = new QLabel("0"));
	oflvl->addWidget(oneFourY_ = new QLabel("0"));
	oflvl->addWidget(oneFourZ_ = new QLabel("0"));
	oflvl->addStretch();
	oneFourLabelsFrame->setLayout(oflvl);

	QFrame* twoOneLabelsFrame = new QFrame();
	QVBoxLayout *tolvl = new QVBoxLayout();
	tolvl->setContentsMargins(12,4,12,4);
	tolvl->addWidget(twoOneX_ = new QLabel("0"));
	tolvl->addWidget(twoOneY_ = new QLabel("0"));
	tolvl->addWidget(twoOneZ_ = new QLabel("0"));
	tolvl->addStretch();
	twoOneLabelsFrame->setLayout(tolvl);

	QFrame* twoTwoLabelsFrame = new QFrame();
	QVBoxLayout *ttlvl = new QVBoxLayout();
	ttlvl->setContentsMargins(12,4,12,4);
	ttlvl->addWidget(twoTwoX_ = new QLabel("0"));
	ttlvl->addWidget(twoTwoY_ = new QLabel("0"));
	ttlvl->addWidget(twoTwoZ_ = new QLabel("0"));
	ttlvl->addStretch();
	twoTwoLabelsFrame->setLayout(ttlvl);

	QFrame* twoThreeLabelsFrame = new QFrame();
	QVBoxLayout *tThlvl = new QVBoxLayout();
	tThlvl->setContentsMargins(12,4,12,4);
	tThlvl->addWidget(twoThreeX_ = new QLabel("0"));
	tThlvl->addWidget(twoThreeY_ = new QLabel("0"));
	tThlvl->addWidget(twoThreeZ_ = new QLabel("0"));
	tThlvl->addStretch();
	twoThreeLabelsFrame->setLayout(tThlvl);


	QFrame* twoFourLabelsFrame = new QFrame();
	QVBoxLayout *tflvl = new QVBoxLayout();
	tflvl->setContentsMargins(12,4,12,4);
	tflvl->addWidget(twoFourX_ = new QLabel("0"));
	tflvl->addWidget(twoFourY_ = new QLabel("0"));
	tflvl->addWidget(twoFourZ_ = new QLabel("0"));
	tflvl->addStretch();
	twoFourLabelsFrame->setLayout(tflvl);

	oodhl->addWidget(oneOneLabelsFrame);
	oodhl->addStretch();
	oneOneDataFrame->setLayout(oodhl);

	otdhl->addWidget(oneTwoLabelsFrame);
	otdhl->addStretch();
	oneTwoDataFrame->setLayout(otdhl);

	oThdhl->addWidget(oneThreeLabelsFrame);
	oThdhl->addStretch();
	oneThreeDataFrame->setLayout(oThdhl);

	ofdhl->addWidget(oneFourLabelsFrame);
	ofdhl->addStretch();
	oneFourDataFrame->setLayout(ofdhl);

	todhl->addWidget(twoOneLabelsFrame);
	todhl->addStretch();
	twoOneDataFrame->setLayout(todhl);

	ttdhl->addWidget(twoTwoLabelsFrame);
	ttdhl->addStretch();
	twoTwoDataFrame->setLayout(ttdhl);

	tThdhl->addWidget(twoThreeLabelsFrame);
	tThdhl->addStretch();
	twoThreeDataFrame->setLayout(tThdhl);

	tfdhl->addWidget(twoFourLabelsFrame);
	tfdhl->addStretch();
	twoFourDataFrame->setLayout(tfdhl);

	oovl->addWidget(oneOneDataFrame);
	oovl->addWidget(oneFourDataFrame);
	oovl->addStretch();
	oneOneFrame->setLayout(oovl);

	otvl->addWidget(oneTwoDataFrame);
	otvl->addWidget(oneThreeDataFrame);
	otvl->addStretch();
	oneTwoFrame->setLayout(otvl);

	tovl->addWidget(twoOneDataFrame);
	tovl->addWidget(twoFourDataFrame);
	tovl->addStretch();
	twoOneFrame->setLayout(tovl);

	ttvl->addWidget(twoTwoDataFrame);
	ttvl->addWidget(twoThreeDataFrame);
	ttvl->addStretch();
	twoTwoFrame->setLayout(ttvl);

	ohl->addWidget(oneOneFrame);
	ohl->addWidget(oneTwoFrame);
	ohl->addWidget(oneButtonFrame);
	oneFrame->setLayout(ohl);

	thl->addWidget(twoOneFrame);
	thl->addWidget(twoTwoFrame);
	thl->addWidget(twoButtonFrame);
	twoFrame->setLayout(thl);


	//    QFrame *beamSelectionFrame = new QFrame();
	//    QHBoxLayout *bhl = new QHBoxLayout();
	//    bhl->setContentsMargins(12,4,12,4);
	//    bhl->addWidget(beamSelectionBox_ = new ComboBox());

	QFrame* buttonFrame = new QFrame();
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->setContentsMargins(12,4,12,4);
	buttonLayout->addWidget(intersectionButton_ = new QPushButton("Intersection"));
	buttonFrame->setLayout(buttonLayout);

	QFrame* dbFrame = new QFrame();
	QHBoxLayout* dbLayout = new QHBoxLayout();
	dbLayout->setContentsMargins(12,4,12,4);
	dbLayout->addWidget(beamSelectionBox_ = new QComboBox());
	dbLayout->addWidget(beamConfigurationName_ = new QLineEdit());
	dbLayout->addWidget(beamSaveConfiguration_ = new QPushButton("Save Beam Configuration"));
	dbLayout->addStretch();
	dbFrame->setLayout(dbLayout);

	beamSelectionBox_->setLineEdit(beamConfigurationName_);

	vbl->addWidget(optionFrame);
	vbl->addWidget(oneFrame);
	vbl->addWidget(twoFrame);
	vbl->addStretch();
	vbl->addWidget(buttonFrame);
	vbl->addWidget(dbFrame);
	setLayout(vbl);




	// make connections

	connect(divergentBox_, SIGNAL(stateChanged(int)), this, SLOT(divergentChecked(int)));
	connect(oneSelect_, SIGNAL(clicked()), this, SIGNAL(oneSelect()));
	connect(twoSelect_, SIGNAL(clicked()), this, SLOT(selectTwo()));
	connect(intersectionButton_, SIGNAL(clicked()), this, SIGNAL(intersection()));

	connect(beamSaveConfiguration_, SIGNAL(clicked()), this, SLOT(saveBeamConfiguration()));
	connect(beamSelectionBox_, SIGNAL(activated(int)), this, SLOT(loadBeamConfiguration(int)));

	updateData();

}

void AMBeamConfigurationView::beamChanged(QObject *newBeam)
{
	AMBeamConfiguration *beam = qobject_cast<AMBeamConfiguration*>(newBeam);
	if(beam != 0)
	{
		beamModel_ = beam;
		updateData();
	}
	else
		AMErrorMon::alert(this, AMBEAMCONFIGURATIONVIEW_CANNOT_ASSIGN_NULL_BEAM, QString("AMBeamConfigurationView was asked to set the beam model to a null object, this is not possible.") );
}

void AMBeamConfigurationView::saveBeamConfiguration()
{
	beamModel_->storeToDb(AMDatabase::database("SGMPublic"));
}

void AMBeamConfigurationView::loadBeamConfiguration(int index)
{
	AMDatabase* db = AMDatabase::database("SGMPublic");
	QList<int> matchList = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"id",index + 1);
	if(!matchList.isEmpty())
	{
		beamModel_->loadFromDb(db,index + 1);
		updateData();
	}
}

void AMBeamConfigurationView::divergentChecked(int checked)
{
	QString divergent = "Select Shape";
	QString nonDivergent = "Select Point ";
	if(checked) divergent_ = true;
	else divergent_ = false;
	if(divergent_) twoSelect_->setText(divergent);
	else twoSelect_->setText(nonDivergent);
}

void AMBeamConfigurationView::selectTwo()
{
	emit twoSelect();
	if(!divergent_)
	{
		beamModel_->alignPositionTwo();
	}
}


void AMBeamConfigurationView::updateData()
{
	QVector<QVector3D> positionOne = beamModel_->positionOne();
	QVector<QVector3D> positionTwo = beamModel_->positionTwo();
	for(int i = 0; i < 4; i++){
		if(!positionOne.isEmpty() && positionOne.count() > i && !positionOne[i].isNull()){

			switch(i)
			{
			case 0: updateOneOne(positionOne[i]);
				break;
			case 1: updateOneTwo(positionOne[i]);
				break;
			case 2: updateOneThree(positionOne[i]);
				break;
			case 3: updateOneFour(positionOne[i]);
				break;
			}
		}
		else
			AMErrorMon::alert(this, AMBEAMCONFIGURATIONVIEW_UPDATE_DATA_INVALID_FIRST_SHAPE, QString("AMBeamConfigurationView cannot updateData() because the first shape is invalid.") );

		if(!positionTwo.isEmpty() && positionTwo.count() > i && !positionTwo[i].isNull()){
			switch(i)
			{
			case 0: updateTwoOne(positionTwo[i]);
				break;
			case 1: updateTwoTwo(positionTwo[i]);
				break;
			case 2: updateTwoThree(positionTwo[i]);
				break;
			case 3: updateTwoFour(positionTwo[i]);
				break;
			}

		}
		else
			AMErrorMon::alert(this, AMBEAMCONFIGURATIONVIEW_UPDATE_DATA_INVALID_SECOND_SHAPE, QString("AMBeamConfigurationView cannot updateData() because the second shape is invalid.") );
	}
	populateBeamSelectionBox();




}

void AMBeamConfigurationView::updateOneOne(QVector3D one)
{
	QString x = QString::number(one.x());
	QString y = QString::number(one.y());
	QString z = QString::number(one.z());

	oneOneX_->setText(x);
	oneOneY_->setText(y);
	oneOneZ_->setText(z);
}

void AMBeamConfigurationView::updateOneTwo(QVector3D two)
{
	QString x = QString::number(two.x());
	QString y = QString::number(two.y());
	QString z = QString::number(two.z());

	oneTwoX_->setText(x);
	oneTwoY_->setText(y);
	oneTwoZ_->setText(z);
}

void AMBeamConfigurationView::updateOneThree(QVector3D three)
{
	QString x = QString::number(three.x());
	QString y = QString::number(three.y());
	QString z = QString::number(three.z());

	oneThreeX_->setText(x);
	oneThreeY_->setText(y);
	oneThreeZ_->setText(z);
}

void AMBeamConfigurationView::updateOneFour(QVector3D four)
{
	QString x = QString::number(four.x());
	QString y = QString::number(four.y());
	QString z = QString::number(four.z());

	oneFourX_->setText(x);
	oneFourY_->setText(y);
	oneFourZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoOne(QVector3D one)
{
	QString x = QString::number(one.x());
	QString y = QString::number(one.y());
	QString z = QString::number(one.z());

	twoOneX_->setText(x);
	twoOneY_->setText(y);
	twoOneZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoTwo(QVector3D two)
{
	QString x = QString::number(two.x());
	QString y = QString::number(two.y());
	QString z = QString::number(two.z());

	twoTwoX_->setText(x);
	twoTwoY_->setText(y);
	twoTwoZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoThree(QVector3D three)
{
	QString x = QString::number(three.x());
	QString y = QString::number(three.y());
	QString z = QString::number(three.z());

	twoThreeX_->setText(x);
	twoThreeY_->setText(y);
	twoThreeZ_->setText(z);
}

void AMBeamConfigurationView::updateTwoFour(QVector3D four)
{
	QString x = QString::number(four.x());
	QString y = QString::number(four.y());
	QString z = QString::number(four.z());

	twoFourX_->setText(x);
	twoFourY_->setText(y);
	twoFourZ_->setText(z);
}

void AMBeamConfigurationView::populateBeamSelectionBox()
{
	beamSelectionBox_->clear();
	AMDatabase* db = AMDatabase::database("SGMPublic");
	QList<QVariant> matchList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMBeamConfiguration>(),"name");
	foreach(QVariant matchItem, matchList)
		beamSelectionBox_->addItem(matchItem.toString());
}


