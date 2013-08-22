#include "ui/dataman/AMSampleView.h"

#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>
#include <QTextEdit>
#include "ui/AMElementListEdit.h"
#include "ui/util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "ui/util/AMPeriodicTableView.h"
#include "ui/beamline/camera/AMShapeDataView.h"

#include "dataman/AMSample.h"
#include "ui/util/AMSamplePeriodicTableDialog.h"
//#include "dataman/AMSamplePlate.h"


AMSampleView::AMSampleView(QWidget* parent)
    : QWidget(parent)
{
    qDebug()<<"Creating new AMSampleView 1";
    sample_ = 0;
    setUpGui();
    makeConnections();
    loadFromDb();

}

AMSampleView::AMSampleView(AMSample *sample, QWidget *parent)
    : QWidget(parent)
{
    qDebug()<<"Creating new AMSampleView 2";
    sample_ = sample;
    setUpGui();
    makeConnections();
    loadFromDb();
}

void AMSampleView::setSample(AMSample *sample)
{
    if(sample_ != sample)
    {
        sample_ = sample;
    //    shapeDataView_->setShapeData(sample->sampleShapePositionData());
    //    connect(sample_, SIGNAL(sampleNameChanged(QString)), nameText_, SLOT(setText(QString)));
            connect(sample_, SIGNAL(nameChanged(QString)), this, SLOT(onSampleNameChanged(QString)));
            connect(this, SIGNAL(updateName(QString)), nameText_, SLOT(setText(QString)));
        updateFrames();
    }
}

void AMSampleView::setName(QString name)
{
    sample_->setName(name);
}

void AMSampleView::setDateTime(QString dateTime)
{
    QDateTime sampleDateTime;
    sampleDateTime.fromString(dateTime,"MMM d  (yyyy)");
    sample_->setDateTime(sampleDateTime);
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
    updateFrames();
}

void AMSampleView::showPeriodicTable()
{
    const AMElement* element = AMSamplePeriodicTableDialog::getElement(sample_->elements());

    if(element)
        sample_->toggleElement(element);


    updateFrames();
}

void AMSampleView::updateSampleName(QString name)
{
    nameText_->setText(name);
    if(sample_ && name != sample_->name())
    {
        qDebug()<<"AMSampleView::updateSampleName - sample has a different name than what was passed in";
        qDebug()<<"Sample's name is:"<<sample_->name();
        qDebug()<<"Updated name is:"<<name;
    }
}



void AMSampleView::updateFrames()
{
    if(sample_)
    {
        nameText_->setText(sample_->name());
        dateTimeText_->setText(sample_->dateTimeString());
        tagText_->setText(sample_->tags().join(", "));
        notesText_->setText(sample_->notes());
        elementsText_->setText(sample_->elementString());
        samplePlateName_->setText(sample_->samplePlateName());
    }
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

    if(sample_)
    {
        QStringList tagList = sample_->tags();
        QStringListModel* tagListModel = new QStringListModel(tagList);
        tagBox_->setModel(tagListModel);
    }

}


void AMSampleView::loadSample(QString sampleName)
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
    updateFrames();
}

//void AMSampleView::changeSamplePlate(QString name)
//{
//    AMDatabase* db = AMDatabase::database("user");
//    /// load the sample plate, make sure it's the same one, set it as sample_'s samplePlate_.
//    QList<int> comboBoxList;
//    int dbIndex;
//    for(int i = 0; i < samplePlateLoader_->count(); i++)
//    {
//        if(samplePlateLoader_->itemText(i) == name)
//            comboBoxList<<i;
//    }
//    QList<int> matchIds = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlatePre2013>(),"name", name);
//    if(matchIds.count() < 1)
//    {
//        qDebug()<<"AMSampleEthanView::changeSamplePlate - Error in sample plate selector, no matching sample plate";
//        return;
//    }
//    else if(matchIds.count() > 1)
//    {
//        if(matchIds.count() != comboBoxList.count())
//        {
//            qDebug()<<"AMSampleEthanView::changeSamplePlate - inconsistent count in loader and database";
//            return;
//        }
//        int index = comboBoxList.indexOf(samplePlateLoader_->currentIndex());
//        dbIndex = matchIds.at(index);
//    }
//    else
//    {
//        dbIndex = matchIds.first();
//    }
//    AMSamplePlatePre2013* samplePlate = new AMSamplePlatePre2013();
//    samplePlate->loadFromDb(db,dbIndex);
//    sample_->setSamplePlate(samplePlate);

//    updateFrames();
//}

void AMSampleView::onSampleNameChanged(QString name)
{
    qDebug()<<"AMSampleView::onSampleNameChanged - recieved name changed signal";
    qDebug()<<"changing sample view name box to "<<name;
    emit updateName(name);
}




void AMSampleView::setUpGui()
{
    qDebug()<<"Called setupgui";
    shapeDataView_ = AMShapeDataView::shapeView();

    QStringList wordList;
    wordList_ = new QStringListModel(wordList);
    completer_ = new QCompleter();
    completer_->setModel(wordList_);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);

    /// set up gui
    QFrame* sampleFrame = new QFrame();
    QVBoxLayout* sampleViewLayout = new QVBoxLayout();
    sampleViewLayout->setContentsMargins(12,4,12,4);
    sampleViewLayout->addWidget(nameText_ = new QLineEdit());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(dateTimeText_ = new QLineEdit());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(tagBox_ = new QComboBox());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(removeTagButton_ = new QPushButton("Remove Tag"));
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(notesText_ = new QTextEdit());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(elementsText_ = new QLineEdit());;
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(showElementDialog_ = new QPushButton("Show element dialog"));
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(samplePlateName_ = new QLineEdit());
    sampleViewLayout->addSpacing(20);
    //    sampleViewLayout->addWidget(samplePlateLoader_ = new QComboBox());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(sampleLoader_ = new QComboBox());
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(saveToDb_ = new QPushButton("Save to Database"));
    sampleViewLayout->addStretch();

    sampleFrame->setLayout(sampleViewLayout);

    QHBoxLayout* viewLayout = new QHBoxLayout();
    viewLayout->setContentsMargins(0,0,0,0);
    viewLayout->addWidget(sampleFrame);
    viewLayout->addWidget(shapeDataView_);
    viewLayout->addStretch();

    setLayout(viewLayout);


    tagText_ = new QLineEdit();

    tagBox_->setCompleter(completer_);

    tagText_->setCompleter(completer_);

    tagBox_->setLineEdit(tagText_);


    updateFrames();

//    populateSamplePlateLoader();

    populateSampleLoader();

}

void AMSampleView::makeConnections()
{
    connect(nameText_, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
    connect(dateTimeText_, SIGNAL(textEdited(QString)), this, SLOT(setDateTime(QString)));
    connect(tagText_, SIGNAL(returnPressed()), this, SLOT(addTag()));
    connect(removeTagButton_, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(notesText_, SIGNAL(textChanged()), this, SLOT(setNotes()));
//    connect(samplePlateLoader_, SIGNAL(currentIndexChanged(QString)), this, SLOT(changeSamplePlate(QString)));
    connect(saveToDb_, SIGNAL(clicked()), this, SLOT(saveToDb()));
    connect(sampleLoader_, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadSample(QString)));
    connect(showElementDialog_, SIGNAL(clicked()), this, SLOT(showPeriodicTable()));
    connect(sample_, SIGNAL(nameChanged(QString)), this, SLOT(onSampleNameChanged(QString)));
    connect(this, SIGNAL(updateName(QString)), nameText_, SLOT(setText(QString)));
}

//void AMSampleView::populateSamplePlateLoader()
//{
//    samplePlateLoader_->blockSignals(true);
//    samplePlateLoader_->clear();
//    samplePlateLoader_->blockSignals(false);
//    AMDatabase* db = AMDatabase::database("user");
//    QList<QVariant> nameList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMSamplePlatePre2013>(), "name");
//    foreach(QVariant item, nameList)
//    {
//        samplePlateLoader_->addItem(item.toString());
//    }


//}

#include "beamline/camera/AMShapeData.h"
void AMSampleView::loadFromDb()
{


    AMDatabase* db = AMDatabase::database("user");
    if(sample_ == 0)
    {
        QString defaultName = "defaultSample";
        QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSample>(), "name", defaultName);
        if(matchIDs.count() == 0)
        {
            sample_ = new AMSample();
            sample_->setName(defaultName);
            sample_->setCurrentDateTime();
            sample_->setNotes("Default Notes");
            bool success = sample_->storeToDb(db);
            if(!success)qDebug()<<"AMSampleEthanView::loadFromDb - failed to store new item in database";
        }
        else
        {
            sample_ = new AMSample();
            bool success = sample_->loadFromDb(db,matchIDs.first());
            if(!success) qDebug()<<"AMSampleEthanView::loadFromDb - failed to load item from database";
        }
    }
    else
    {
        bool success = sample_->storeToDb(db);
        if(!success)qDebug()<<"AMSampleEthanView::loadFromDb - failed to store existing item in database";
    }
}

void AMSampleView::populateSampleLoader()
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
        // samples are in a different order...
        qDebug()<<"populateSampleLoader - Samples out of order";
        sampleLoader_->setCurrentIndex(-1);
    }
}

void AMSampleView::saveToDb()
{
    AMDatabase* db = AMDatabase::database("user");
    bool success = sample_->storeToDb(db);
    if(!success) qDebug()<<"AMSampleEthanView::saveToDb - failed to store item to db";
    populateSampleLoader();
    updateFrames();
}


