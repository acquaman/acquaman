
#include "AMNewRunDialog.h"


//Constructor:

AMNewRunDialog:: AMNewRunDialog(QWidget *parent)
	: QWidget(parent)
{
	database_ = AMDatabase::userdb();
	//setting up the layout of the entire window

	QLabel *facilitiesLabel = new QLabel(tr("Facilities"));
	QLabel *runNameLineLabel = new QLabel(tr("New Run Name:"));
	QPushButton *okButton = new QPushButton("Okay");
	QPushButton *cancelButton = new QPushButton("Cancel");
	facilitySelectCb = new QComboBox();
	runNameLineEdit = new QLineEdit();



	addRunsAndFacilitiesLayout = new QGridLayout;

	addRunsAndFacilitiesLayout->addWidget(facilitiesLabel,0,0);
	addRunsAndFacilitiesLayout ->addWidget(facilitySelectCb, 0, 1);
	addRunsAndFacilitiesLayout ->addWidget(runNameLineLabel,1,0);
	addRunsAndFacilitiesLayout ->addWidget(runNameLineEdit, 1, 1, 1, 2);
	addRunsAndFacilitiesLayout ->addWidget(okButton, 2, 2);
	addRunsAndFacilitiesLayout ->addWidget(cancelButton, 2, 3);
	setLayout(addRunsAndFacilitiesLayout);

	addFacility(); // added all facilities in database to the combo box
	//facilitySelectCbChanged(0);

	connect(okButton,SIGNAL(clicked()), this, SLOT(okButtonPressed()));

//when user selects a different facility in the combobox, must append facility name to line edit
	connect(facilitySelectCb, SIGNAL(activated(int)),this, SLOT(facilitySelectCbChanged(int)));

	connect(cancelButton,SIGNAL(clicked()),this, SLOT(cancelButtonPressed()));
	AMRun("test",0);

}


AMNewRunDialog::~AMNewRunDialog()
{
}


void AMNewRunDialog::addFacility(){

	//Before adding anything into the combobox, make sure nothing is in it
	facilitySelectCb->clear();

	// searching database for the required components
	QSqlQuery q = database_->query();

	//q.prepare(QString("SELECT Facilities.description FROM Facilities" ));

	q.prepare(QString("SELECT Facilities.description,Facilities.name,Thumbnails.thumbnail,Thumbnails.type,Facilities.id "
					  "FROM Facilities,Thumbnails WHERE Facilities.thumbnailFirstId = Thumbnails.id "
					  "ORDER BY Facilities.description ASC"));

	int i = 0;
	if (q.exec()) {
		while (q.next()) {
			facilitySelectCb->addItem(QString(q.value(1).toString()));  //Adding facilities name

			if(q.value(3).toString() == "PNG") {      // Checking if thumbnail type is PNG
				QPixmap p;
				if(p.loadFromData(q.value(2).toByteArray(), "PNG"))  // Converting thumbnail to byte array and storing it as decoration role
					facilitySelectCb->setItemData(i, p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}
			facilitySelectCb->setItemData(i,q.value(0),Qt::ToolTipRole);  //Setting description as tool tip
			facilitySelectCb->setItemData(i,q.value(4), Qt::UserRole);		// Setting facility's ID in the User Role
			i++;
		}
	}
	else
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Error retrieving information from the database."));


}



void AMNewRunDialog::facilitySelectCbChanged(int index) {
	facilitySelectCb->setCurrentIndex(index);  //not a necessary line...
	QString text = facilitySelectCb->currentText();
	runNameLineEdit->clear();
	runNameLineEdit->insert(text+"--");
}

//This function is activated when the okay button is pressed, and will store the contents of the line edit as the name of the new run and the current facility's id as the new run's facility id
void AMNewRunDialog::okButtonPressed(){
	QString runName = runNameLineEdit->text();
	//run AMRun constructor to create new run, but first, we need facility Id
	int facilityId = facilitySelectCb->itemData(facilitySelectCb->currentIndex(),Qt::UserRole).toInt();
	AMRun(runName, facilityId).storeToDb(AMDatabase::userdb());

	hide();

	emit dialogBoxClosed();


}

// this will hide the dialog box if the cancel button is pressed
void AMNewRunDialog::cancelButtonPressed(){
	close();

}
