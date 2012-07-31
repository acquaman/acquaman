#include "AMSampleSelector.h"

#include "dataman/database/AMDatabase.h"
#include "acquaman.h"
#include <QInputDialog>
#include "dataman/AMSample.h"
#include "dataman/database/AMDbObjectSupport.h"
#include <QListView>
#include "ui/AMDetailedItemDelegate.h"
#include <QMessageBox>
#include "util/AMDateTimeUtils.h"

AMSampleSelector::AMSampleSelector(AMDatabase* db, QWidget *parent) :
    QComboBox(parent)
{
	db_ = db;
	tableName_ = AMDbObjectSupport::s()->tableNameForClass<AMSample>();
	currentlyInsertingSample_ = false;

	QListView* lview = new QListView();
	AMDetailedItemDelegate* del = new AMDetailedItemDelegate(this);
	del->setCloseButtonsEnabled(true);
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onSampleDeleteButtonClicked(QModelIndex)));
	lview->setItemDelegate(del);
	lview->setAlternatingRowColors(true);
	setView(lview);

	populateFromDb();

	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseRowAdded(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRowRemoved(QString,int)));
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseRowUpdated(QString,int)));

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
	connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
}

void AMSampleSelector::onCurrentIndexChanged(int)
{
	emit currentSampleChanged(currentSample());

	// currentIndex == 0 will be handled in onActivated().
}

void AMSampleSelector::populateFromDb()
{
	clear();

	addItem("Create New Sample...");
	setItemData(0, -1, AM::IdRole);

	QSqlQuery q = db_->query();
	q.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime DESC;").arg(tableName_));
	q.exec();
	while(q.next()) {
		int id = q.value(0).toInt();
		QString name = q.value(1).toString();
		QDateTime dateTime = q.value(2).toDateTime();

		addItem(name);
		setItemData(count()-1, id, AM::IdRole);
		setItemData(count()-1, dateTime, AM::DateTimeRole);
		setItemData(count()-1, QString("created %1").arg(AMDateTimeUtils::prettyDateTime(dateTime)), AM::DescriptionRole);
	}
}

void AMSampleSelector::onDatabaseRowUpdated(const QString &tableName, int row)
{
	if(tableName != tableName_)
		return;

	// find the sample based on its id.
	int index = findData(row, AM::IdRole);
	// and update it.
	if(index != -1) {
		AMSample s;
		s.loadFromDb(db_, row);

		setItemText(index, s.name());
		setItemData(index, s.dateTime(), AM::DateTimeRole);
	}
}

void AMSampleSelector::onDatabaseRowAdded(const QString &tableName, int row)
{
	if(tableName != tableName_)
		return;

	AMSample s;

	// We can count that this will be the most recent, so insert it at the beginning.
	insertItem(1, s.name());
	setItemData(1, row, AM::IdRole);
	setItemData(1, s.dateTime(), AM::DateTimeRole);

	if(currentlyInsertingSample_) {
		currentlyInsertingSample_ = false;
		setCurrentIndex(1);
	}
}

void AMSampleSelector::onDatabaseRowRemoved(const QString &tableName, int row)
{
	if(tableName != tableName_)
		return;

	// find the removed-sample based on its id.
	int index = findData(row, AM::IdRole);
	// and remove it.
	if(index != -1) {
		removeItem(index);
	}
}


void AMSampleSelector::showAddSampleDialog()
{
	bool ok;
	QString newSampleName = QInputDialog::getText(this, "New Sample", "Please enter a name for the new sample", QLineEdit::Normal, "New Sample 1", &ok);
	if(ok) {
		currentlyInsertingSample_ = true;
		AMSample s(newSampleName);
		s.setDateTime(QDateTime::currentDateTime());
		s.storeToDb(db_);
	}
}

void AMSampleSelector::setCurrentSample(int id)
{
	if(id < 1)
		setCurrentIndex(-1);

	int index = findData(id, AM::IdRole);
	setCurrentIndex(index);
}

int AMSampleSelector::currentSample() const
{
	if(currentIndex() < 1)
		return -1;

	return itemData(currentIndex(), AM::IdRole).toInt();
}

void AMSampleSelector::onActivated(int currentIndex)
{
	if(currentIndex == 0)
		showAddSampleDialog();
}

void AMSampleSelector::onSampleDeleteButtonClicked(const QModelIndex &index) {

	if(!index.isValid() || index.row() < 1)
		return;

	int sampleId = itemData(index.row(), AM::IdRole).toInt();

	QString sampleName = itemData(index.row(), Qt::DisplayRole).toString();
	QString dt = AMDateTimeUtils::prettyDateTime(itemData(index.row(), AM::DateTimeRole).toDateTime());

	QMessageBox confirmBox(this);
	confirmBox.setText(QString("Delete the sample '%1'', created %2?").arg(sampleName).arg(dt));
	confirmBox.setInformativeText("All scans on this sample will be kept, but they will lose their affiliation with this sample.");
	confirmBox.setIcon(QMessageBox::Question);
	confirmBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	if(confirmBox.exec() == QMessageBox::Ok)
		AMSample::destroySample(db_, sampleId);
}
