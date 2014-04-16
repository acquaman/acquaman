#include "ui/dataman/AMSampleView.h"

#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>
#include <QTextEdit>
#include <QLabel>

#include "ui/AMElementListEdit.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "ui/util/AMPeriodicTableView.h"
#include "ui/beamline/camera/AMShapeDataView.h"

#include "dataman/AMSample.h"
#include "ui/util/AMSamplePeriodicTableDialog.h"
#include "dataman/AMSamplePlate.h"

AMSampleReadOnlyView::AMSampleReadOnlyView(const AMSample *sample, QWidget *parent) :
	QWidget(parent)
{
	constSample_ = sample;

	QStringList wordList;
	wordList_ = new QStringListModel(wordList);
	completer_ = new QCompleter();
	completer_->setModel(wordList_);
	completer_->setCaseSensitivity(Qt::CaseInsensitive);

	sampleViewLayout_ = new QVBoxLayout();
	sampleViewLayout_->setContentsMargins(12,4,12,4);

	QHBoxLayout *tempHL;
	QVBoxLayout *tempVL;

	tempHL = new QHBoxLayout();
	nameText_ = new QLineEdit();
	nameText_->setEnabled(false);
	tempHL->addWidget(new QLabel("Name:"));
	tempHL->addWidget(nameText_);
	sampleViewLayout_->addLayout(tempHL);
	sampleViewLayout_->addSpacing(8);

	tempHL = new QHBoxLayout();
	dateTimeText_ = new QLineEdit();
	dateTimeText_->setEnabled(false);
	tempHL->addWidget(new QLabel("Created:"));
	tempHL->addWidget(dateTimeText_);
	sampleViewLayout_->addLayout(tempHL);
	sampleViewLayout_->addSpacing(8);

	tempHL = new QHBoxLayout();
	tagBox_ = new QComboBox();
	tagBox_->setEnabled(false);
	tempHL->addWidget(new QLabel("Tags:"));
	tempHL->addWidget(tagBox_);
	sampleViewLayout_->addLayout(tempHL);
	tagLayoutItem_ = sampleViewLayout_->itemAt(sampleViewLayout_->count()-1);
	sampleViewLayout_->addSpacing(8);

	tempVL = new QVBoxLayout();
	notesText_ = new QTextEdit();
	notesText_->setEnabled(false);
	tempVL->addWidget(new QLabel("Notes:"));
	tempVL->addWidget(notesText_);
	sampleViewLayout_->addLayout(tempVL);
	sampleViewLayout_->addSpacing(8);

	tempHL = new QHBoxLayout();
	elementsText_ = new QLineEdit();
	elementsText_->setEnabled(false);
	tempHL->addWidget(new QLabel("Elements:"));
	tempHL->addWidget(elementsText_);
	sampleViewLayout_->addLayout(tempHL);
	elementLayoutItem_ = sampleViewLayout_->itemAt(sampleViewLayout_->count()-1);
	sampleViewLayout_->addSpacing(8);

	tempHL = new QHBoxLayout();
	samplePlateName_ = new QLineEdit();
	samplePlateName_->setEnabled(false);
	tempHL->addWidget(new QLabel("Sample Plate:"));
	tempHL->addWidget(samplePlateName_);
	sampleViewLayout_->addLayout(tempHL);
	sampleViewLayout_->addSpacing(8);

	tagText_ = new QLineEdit();
	tagText_->setEnabled(false);
	tagBox_->setCompleter(completer_);
	tagText_->setCompleter(completer_);
	tagBox_->setLineEdit(tagText_);

	setLayout(sampleViewLayout_);

	loadTagsFromDatabase();

	initializeSampleViewData();
}

void AMSampleReadOnlyView::initializeSampleViewData(){
	if(constSample_){
		nameText_->setText(constSample_->name());
		dateTimeText_->setText(constSample_->dateTimeString());
		tagText_->setText(constSample_->tags().join(", "));
		notesText_->setText(constSample_->notes());
		elementsText_->setText(constSample_->elementString());
		samplePlateName_->setText(constSample_->samplePlate()->name());

		QStringList tagList = constSample_->tags();
		QStringListModel* tagListModel = new QStringListModel(tagList);
		tagBox_->setModel(tagListModel);
	}
	else{
		nameText_->setText("");
		dateTimeText_->setText("");
		tagText_->setText("");
		notesText_->setText("");
		elementsText_->setText("");
		samplePlateName_->setText("");

		QStringList tagList;
		QStringListModel* tagListModel = new QStringListModel(tagList);
		tagBox_->setModel(tagListModel);
	}
}

void AMSampleReadOnlyView::loadTagsFromDatabase(){
	AMDatabase* db = AMDatabase::database("user");
	QList<QVariant> matchIDs = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "tags");
	QStringList itemList;
	foreach(QVariant item, matchIDs)
	{
		QStringList subList = item.toString().split(AMDbObjectSupport::stringListSeparator());
		foreach(QString subString, subList)
		{
			if(!itemList.contains(subString))
				itemList<<subString;
		}


	}
	wordList_->setStringList(itemList);
}

AMSampleView::AMSampleView(AMSample *sample, QWidget *parent) :
	AMSampleReadOnlyView(sample, parent)
{
	sample_ = 0; //NULL

	/// set up gui
	removeTagButton_ = new QPushButton("Remove Tag");
	for(int x = 0, size = sampleViewLayout_->count(); x < size; x++)
		if(sampleViewLayout_->itemAt(x) == tagLayoutItem_)
			sampleViewLayout_->insertWidget(x+1, removeTagButton_);

	showElementDialog_ = new QPushButton("Show element dialog");
	for(int x = 0, size = sampleViewLayout_->count(); x < size; x++)
		if(sampleViewLayout_->itemAt(x) == elementLayoutItem_)
			sampleViewLayout_->insertWidget(x+1, showElementDialog_);

	sampleViewLayout_->addWidget(saveToDb_ = new QPushButton("Save to Database"));
	sampleViewLayout_->addStretch();

	nameText_->setEnabled(true);
	notesText_->setEnabled(true);
	tagText_->setEnabled(true);
	tagBox_->setEnabled(true);
	elementsText_->setEnabled(true);

	connect(nameText_, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
	connect(tagText_, SIGNAL(returnPressed()), this, SLOT(addTag()));
	connect(tagBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentTag()));
	connect(removeTagButton_, SIGNAL(clicked()), this, SLOT(removeTag()));
	connect(notesText_, SIGNAL(textChanged()), this, SLOT(setNotes()));
	connect(saveToDb_, SIGNAL(clicked()), this, SLOT(saveToDb()));
	connect(showElementDialog_, SIGNAL(clicked()), this, SLOT(showPeriodicTable()));
	setSample(sample);

	initializeSampleViewData();
}

void AMSampleView::setSample(AMSample *sample)
{
	if(sample && sample_ != sample)
	{
		if(sample_)
		{
			disconnect(sample_, SIGNAL(nameChanged(QString)), nameText_, SLOT(setText(QString)));
			disconnect(sample_, SIGNAL(requestCurrentTag()), this, SLOT(setCurrentTag()));
			disconnect(sample_, SIGNAL(tagsChanged(QStringList)), this, SLOT(updateTags(QStringList)));
			disconnect(sample_, SIGNAL(sampleShapeDataChanged()), this, SLOT(onSampleShapeDataChanged()));
			disconnect(sample_, SIGNAL(sampleAboutToBeRemoved()), this, SLOT(onSampleAboutToBeRemoved()));
		}
		sample_ = sample;
		loadTagsFromDatabase();
		if(sample_)
		{
			connect(sample_, SIGNAL(nameChanged(QString)), nameText_, SLOT(setText(QString)));
			connect(sample_, SIGNAL(requestCurrentTag()), this, SLOT(setCurrentTag()));
			connect(sample_, SIGNAL(tagsChanged(QStringList)), this, SLOT(updateTags(QStringList)));
			connect(sample_, SIGNAL(sampleShapeDataChanged()), this, SLOT(onSampleShapeDataChanged()));
			connect(sample_, SIGNAL(sampleAboutToBeRemoved()), this, SLOT(onSampleAboutToBeRemoved()));
		}

		initializeSampleViewData();
	}
	else if(!sample)
	{
		sample_ = 0;
	}
}

void AMSampleView::setName(QString name)
{
	sample_->setName(name);
}

void AMSampleView::setNotes()
{
	QString notes = notesText_->toPlainText();
	sample_->setNotes(notes);
}

void AMSampleView::addTag()
{
	QString text = tagText_->text();
	sample_->addTag(text);
	if(!wordList_->stringList().contains(text))
	{
		QStringList stringList(wordList_->stringList());
		stringList<<text;
		wordList_->setStringList(stringList);
	}
	tagText_->setText("");
}

void AMSampleView::removeTag(int index)
{
	sample_->removeTag(tagText_->text());
}

void AMSampleView::removeTag()
{
	removeTag(tagBox_->currentIndex());
	initializeSampleViewData();
}

void AMSampleView::showPeriodicTable()
{
	const AMElement* element = AMSamplePeriodicTableDialog::getElement(sample_->elements());

	if(element)
		sample_->toggleElement(element);

	initializeSampleViewData();
}

void AMSampleView::setCurrentTag()
{
	qDebug()<<"Setting current tag to"<<tagText_->text();
	QString currentTag = tagText_->text();
	if(currentTag == "" && !sample_->tags().isEmpty())
		currentTag = sample_->tags().last();

	sample_->setCurrentTag(currentTag);
}

void AMSampleView::updateTags(QStringList tags)
{
	qDebug()<<"Updating tags";
	//    updateFrames();
	//    QStringListModel* stringModel = qobject_cast<QStringListModel*>(tagBox_->model());
	//    stringModel->setStringList(sample_->tags());
	//    if(stringModel)
	//    {
	//        stringModel->setStringList(tags);
	//        tagBox_->setModel(stringModel);
	//    }
	//    else
	//    {
	//        qDebug()<<"AMSampleView::updateTags - Failed to cast string list";
	//    }

}

void AMSampleView::onSampleAboutToBeRemoved()
{
	close();
	deleteLater();
}

void AMSampleView::initializeSampleViewData(){
	if(sample_)
		AMSampleReadOnlyView::initializeSampleViewData();
}

void AMSampleView::closeEvent(QCloseEvent *event){
	emit aboutToClose();
	QWidget::closeEvent(event);
}

void AMSampleView::saveToDb()
{
	AMDatabase* db = AMDatabase::database("user");
	bool success = sample_->storeToDb(db);
	if(!success) qDebug()<<"AMSampleEthanView::saveToDb - failed to store item to db";
	initializeSampleViewData();
}

AMSampleAdvancedView::AMSampleAdvancedView(AMSample *sample, QWidget *parent) :
	AMSampleView(sample, parent)
{
	shapeDataView_ = new AMShapeDataView(sample_->sampleShapePositionData());
//	viewLayout_->addWidget(shapeDataView_);
	sampleViewLayout_->addWidget(shapeDataView_);

	sampleLoader_ = new QComboBox();
	sampleViewLayout_->addWidget(sampleLoader_);
	sampleViewLayout_->addSpacing(20);
	connect(sampleLoader_, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadSample(QString)));

	if(sample_)
		connect(sample_, SIGNAL(sampleShapeDataChanged()), this, SLOT(onSampleShapeDataChanged()));

	populateSampleLoader();
}

void AMSampleAdvancedView::onSampleShapeDataChanged()
{
	shapeDataView_->setShapeData(sample_->sampleShapePositionData());
}

void AMSampleAdvancedView::loadSample(QString sampleName)
{
	AMDatabase* db = AMDatabase::database("user");
	QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "name", sampleName);
	if(matchIDs.count() == 0)
	{
		qDebug()<<"AMSampleEthanView::loadSample - No match found for sample: "<<sampleName;
	}
	else
	{
		int sampleId;
		if(matchIDs.count() > 1)
		{
			int currentIndex = sampleLoader_->currentIndex();
			QList<int> indexList;
			for(int i = 0; i < sampleLoader_->count(); i++)
			{
				if(sampleLoader_->itemText(i) == sampleName)
					indexList<<i;
			}
			int relativeIndex = indexList.indexOf(currentIndex);

			sampleId = relativeIndex;
		}
		else
		{
			sampleId = 0;
		}
		bool success = sample_->loadFromDb(db,matchIDs.at(sampleId));
		if(!success) qDebug()<<"AMSampleEthanView::loadSample - Failed to load sample from database.";
	}
	initializeSampleViewData();
}

void AMSampleAdvancedView::saveToDb(){
	AMSampleView::saveToDb();
	populateSampleLoader();
}

void AMSampleAdvancedView::populateSampleLoader()
{
	int currentIndex = sampleLoader_->currentIndex();
	QString currentSampleName = nameText_->text();
	sampleLoader_->blockSignals(true);
	sampleLoader_->clear();
	sampleLoader_->blockSignals(false);
	AMDatabase* db = AMDatabase::database("user");
	QList<QVariant> nameList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "name");
	foreach(QVariant item, nameList)
	{
		sampleLoader_->addItem(item.toString());
	}
	sampleLoader_->setCurrentIndex(currentIndex);
	if(sampleLoader_->itemText(currentIndex) != currentSampleName)
	{
		// samples are in a different order
		qDebug()<<"AMSampleView::populateSampleLoader - Samples out of order";
		sampleLoader_->setCurrentIndex(-1);
	}
}
