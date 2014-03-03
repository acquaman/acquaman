#include "AMSampleEditor.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "beamline/AMBeamline.h"
#include "dataman/AMSamplePlate.h"
#include "dataman/database/AMDatabase.h"
#include "dataman/AMSample.h"
#include "dataman/AMSamplePre2013.h"
#include "util/AMDateTimeUtils.h"

AMSampleEditor::AMSampleEditor(AMDatabase *database, QWidget *parent) :
	QWidget(parent)
{
	database_ = database;
	currentSample_ = 0; //NULL
	currentSamplePre2013_ = 0; //NULL

	nameLabel_ = new QLabel("No Sample");
	createdLabel_ = new QLabel("No Sample");
	changeSampleButton_ = new QPushButton("Change Sample");
	connect(changeSampleButton_, SIGNAL(clicked()), this, SLOT(onChangeSampleButtonClicked()));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(nameLabel_);
	vl->addWidget(createdLabel_);
	vl->addWidget(changeSampleButton_);
	setLayout(vl);
}

void AMSampleEditor::setSample(const AMSample *sample){
	currentSample_ = sample;
	nameLabel_->setText(currentSample_->name());
	createdLabel_->setText(AMDateTimeUtils::prettyDateTime(currentSample_->dateTime()));
}

//<<<<<<< HEAD
void AMSampleEditor::setSamplePre2013(const AMSamplePre2013 *samplePre2013){
	currentSamplePre2013_ = samplePre2013;
	nameLabel_->setText(currentSamplePre2013_->name());
	createdLabel_->setText(AMDateTimeUtils::prettyDateTime(currentSamplePre2013_->dateTime()));
}
//=======
#include "util/AMPeriodicTable.h"
//QList<int> AMSampleEditor::parseElementString(const QString &elementString) {
//	QList<int> rv;


//	QStringList elements = elementString.split(QRegExp("[\\,\\;\\s]+"), QString::SkipEmptyParts);

//	foreach(QString s, elements) {
//		s = s.toLower();
//		s = s.left(1).toUpper() + s.mid(1);	// Capitalize as Cl, Chlorine, etc.
//		AMElement element;
//		if(!(element = AMPeriodicTable::table()->elementByName(s)).isNull())
//			rv << element.atomicNumber();
//		else if(!(element = AMPeriodicTable::table()->elementBySymbol(s)).isNull())
//			rv << element.atomicNumber();
//	}

//	return rv;
//	/// \todo Validator / red-green hinting for element editor box
//>>>>>>> SGM_Release
//}

void AMSampleEditor::onChangeSampleButtonClicked(){
	QList<AMSample*> allSamples = AMBeamline::bl()->samplePlateBrowser()->allSamples();
	qDebug() << "Samples names: ";
	for(int x = 0; x < allSamples.count(); x++)
		qDebug() << allSamples.at(x)->name();
}
