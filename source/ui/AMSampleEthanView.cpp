#include "AMSampleEthanView.h"

#include <QLineEdit>
#include <QLayout>
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QStringListModel>
#include <QTextEdit>
#include "AMElementListEdit.h"
#include "util/AMPeriodicTableDialog.h"
#include "util/AMPeriodicTable.h"
#include "util/AMPeriodicTableView.h"

#include "AMSampleEthan.h"
#include "util/AMSamplePeriodicTableDialog.h"


AMSampleEthanView::AMSampleEthanView(QWidget* parent)
    : QWidget(parent)
{

    sample_ = 0;
    setUpGui();
    makeConnections();
    loadFromDb();

}

AMSampleEthanView::AMSampleEthanView(AMSampleEthan *sample, QWidget *parent)
    : QWidget(parent)
{
    sample_ = sample;
    setUpGui();
    makeConnections();
    loadFromDb();
}

void AMSampleEthanView::setSample(AMSampleEthan *sample)
{
    sample_ = sample;
    updateFrames();
}

void AMSampleEthanView::setName(QString name)
{
    sample_->setName(name);
}

void AMSampleEthanView::setDateTime(QString dateTime)
{
    QDateTime sampleDateTime;
    sampleDateTime.fromString(dateTime,"MMM d  (yyyy)");
    sample_->setDateTime(sampleDateTime);
}

void AMSampleEthanView::setNotes()
{
    QString notes = notesText_->toPlainText();
    sample_->setNotes(notes);
}

void AMSampleEthanView::addTag()
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

void AMSampleEthanView::removeTag(int index)
{
    sample_->removeTag(tagText_->text());
}

void AMSampleEthanView::removeTag()
{
    removeTag(tagBox_->currentIndex());
    updateFrames();
}

void AMSampleEthanView::showPeriodicTable()
{
    const AMElement* element = AMSamplePeriodicTableDialog::getElement(sample_->elements());

    if(element)
        sample_->toggleElement(element);


    updateFrames();
}

void AMSampleEthanView::updateFrames()
{
    if(sample_)
    {
        nameText_->setText(sample_->name());
        dateTimeText_->setText(sample_->dateTimeString());
        tagText_->setText(sample_->tags().join(", "));
        notesText_->setText(sample_->notes());
        elementsText_->setText(sample_->elementString());
    }
    AMDatabase* db = AMDatabase::database("user");
    QList<QVariant> matchIDs = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>(), "tags");
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


void AMSampleEthanView::loadSample(QString sampleName)
{
    AMDatabase* db = AMDatabase::database("user");
    QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>(), "name", sampleName);
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
//        AMSampleEthan* tempSample = new AMSampleEthan();
        bool success = sample_->loadFromDb(db,matchIDs.at(sampleId));
        if(!success) qDebug()<<"AMSampleEthanView::loadSample - Failed to load sample from database.";
//        else
//        {
//            sample_ = tempSample;
//        }
    }
    updateFrames();
}




void AMSampleEthanView::setUpGui()
{

    QStringList wordList;
    wordList_ = new QStringListModel(wordList);
    completer_ = new QCompleter();
    completer_->setModel(wordList_);
    completer_->setCaseSensitivity(Qt::CaseInsensitive);

    /// set up gui
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
    sampleViewLayout->addWidget(sampleLoader_ = new QComboBox);
    sampleViewLayout->addSpacing(20);
    sampleViewLayout->addWidget(saveToDb_ = new QPushButton("Save to Database"));
    sampleViewLayout->addStretch();

    setLayout(sampleViewLayout);

    tagText_ = new QLineEdit();

    tagBox_->setCompleter(completer_);

    tagText_->setCompleter(completer_);

    tagBox_->setLineEdit(tagText_);


    updateFrames();

    populateSampleLoader();

}

void AMSampleEthanView::makeConnections()
{
    connect(nameText_, SIGNAL(textEdited(QString)), this, SLOT(setName(QString)));
    connect(dateTimeText_, SIGNAL(textEdited(QString)), this, SLOT(setDateTime(QString)));
    connect(tagText_, SIGNAL(returnPressed()), this, SLOT(addTag()));
    connect(removeTagButton_, SIGNAL(clicked()), this, SLOT(removeTag()));
    connect(notesText_, SIGNAL(textChanged()), this, SLOT(setNotes()));
    connect(saveToDb_, SIGNAL(clicked()), this, SLOT(saveToDb()));
    connect(sampleLoader_, SIGNAL(currentIndexChanged(QString)), this, SLOT(loadSample(QString)));
    connect(showElementDialog_, SIGNAL(clicked()), this, SLOT(showPeriodicTable()));
}

void AMSampleEthanView::loadFromDb()
{


    AMDatabase* db = AMDatabase::database("user");
    if(sample_ == 0)
    {
        QString defaultName = "defaultSample";
        QList<int> matchIDs = db->objectsMatching(AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>(), "name", defaultName);
        if(matchIDs.count() == 0)
        {
            sample_ = new AMSampleEthan();
            sample_->setName(defaultName);
            sample_->setCurrentDateTime();
            sample_->setNotes("Default Notes");
            bool success = sample_->storeToDb(db);
            if(!success)qDebug()<<"AMSampleEthanView::loadFromDb - failed to store new item in database";
        }
        else
        {
            sample_ = new AMSampleEthan();
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

void AMSampleEthanView::populateSampleLoader()
{
    int currentIndex = sampleLoader_->currentIndex();
    QString currentSampleName = nameText_->text();
    sampleLoader_->blockSignals(true);
    sampleLoader_->clear();
    sampleLoader_->blockSignals(false);
    AMDatabase* db = AMDatabase::database("user");
    QList<QVariant> nameList = db->retrieve(AMDbObjectSupport::s()->tableNameForClass<AMSampleEthan>(), "name");
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

void AMSampleEthanView::saveToDb()
{
    AMDatabase* db = AMDatabase::database("user");
    bool success = sample_->storeToDb(db);
    if(!success) qDebug()<<"AMSampleEthanView::saveToDb - failed to store item to db";
    populateSampleLoader();
    updateFrames();
}


