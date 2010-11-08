
#include "AMNewRunDialog.h"
#include "AMErrorMonitor.h"

//Constructor:

AMNewRunDialog:: AMNewRunDialog(AMDatabase* db, QWidget *parent)
	: QDialog(parent)
{
	database_ = db;
	//setting up the layout of the entire window

	QLabel *facilitiesLabel = new QLabel(tr("Facilities"));
	QLabel *runNameLineLabel = new QLabel(tr("New Run Name:"));
	QLabel* informationLabel = new QLabel(tr("<b>Create a new run</b><br><br>These runs will be used to organize your data for each visit to the facility.<br>"));
	QLabel* dateWarningLabel = new QLabel(tr("The run date will be automatically added to this name."));
	QPushButton *okButton = new QPushButton("Ok");
	QPushButton *cancelButton = new QPushButton("Cancel");
	facilitySelectCb = new QComboBox();
	runNameLineEdit = new QLineEdit();



	addRunsAndFacilitiesLayout = new QGridLayout;

	addRunsAndFacilitiesLayout->addWidget(informationLabel, 0,0,1,2,Qt::AlignLeft);
	addRunsAndFacilitiesLayout->addWidget(facilitiesLabel,1,0);
	addRunsAndFacilitiesLayout ->addWidget(facilitySelectCb, 1, 1);
	addRunsAndFacilitiesLayout ->addWidget(runNameLineLabel,2,0);
	addRunsAndFacilitiesLayout ->addWidget(runNameLineEdit, 2, 1, 1, 2);
	addRunsAndFacilitiesLayout->addWidget(dateWarningLabel, 3, 1, 1, 2);
	addRunsAndFacilitiesLayout ->addWidget(okButton, 4, 2);
	addRunsAndFacilitiesLayout ->addWidget(cancelButton, 4, 3);
	setLayout(addRunsAndFacilitiesLayout);

	addFacility(); // added all facilities in database to the combo box
	facilitySelectCbChanged(0);
	connect(okButton,SIGNAL(clicked()), this, SLOT(okButtonPressed()));

//when user selects a different facility in the combobox, must append facility name to line edit
	connect(facilitySelectCb, SIGNAL(activated(int)),this, SLOT(facilitySelectCbChanged(int)));

	connect(cancelButton,SIGNAL(clicked()),this, SLOT(cancelButtonPressed()));
}


AMNewRunDialog::~AMNewRunDialog()
{
}

/// This function scans the database for facilities and their thumbnail and adds the facility description and it's corresponding thumbnail to the facilitySelectCb combobox. Other metadata are added as user role or tooltip role.
void AMNewRunDialog::addFacility(){

	//Before adding anything into the combobox, make sure nothing is in it
	facilitySelectCb->clear();

	// searching database for the required components
	QSqlQuery q = database_->query();

	//q.prepare(QString("SELECT Facilities.description FROM Facilities" ));

	q.prepare(QString("SELECT Facilities.description,Facilities.name,Thumbnails.thumbnail,Thumbnails.type,Facilities.id "
					  "FROM Facilities,Thumbnails WHERE Facilities.thumbnailFirstId = Thumbnails.id "
					  "ORDER BY Facilities.id DESC"));

	int i = 0;
	if (q.exec()) {
		while (q.next()) {
			facilitySelectCb->addItem(QString(q.value(0).toString()));  //Adding facilities description

			if(q.value(3).toString() == "PNG") {      // Checking if thumbnail type is PNG
				QPixmap p;
				if(p.loadFromData(q.value(2).toByteArray(), "PNG"))  // Converting thumbnail to byte array and storing it as decoration role
					facilitySelectCb->setItemData(i, p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}
			facilitySelectCb->setItemData(i,q.value(0).toString(), Qt::ToolTipRole);  //Setting description as tool tip
			facilitySelectCb->setItemData(i,q.value(4).toInt(), AM::IdRole);		// Setting facility's ID in the User Role
			facilitySelectCb->setItemData(i, q.value(1).toString(), AM::DescriptionRole);
			i++;
		}
	}
	else
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Error retrieving information from the database."));


}


/// This function takes the text in the combobox and puts it in the runNameLineEdit.
void AMNewRunDialog::facilitySelectCbChanged(int index) {


	runNameLineEdit->setText( facilitySelectCb->itemData(index, AM::DescriptionRole).toString() );
}

/// This function is activated when the okay button is pressed, and will store the contents of the line edit as the name of the new run and the current facility's id as the new run's facility id
void AMNewRunDialog::okButtonPressed(){
	QString runName = runNameLineEdit->text();
	//run AMRun constructor to create new run, but first, we need facility Id
	int facilityId = facilitySelectCb->itemData(facilitySelectCb->currentIndex(),AM::IdRole).toInt();

	AMRun newRun(runName, facilityId);
	bool success = newRun.storeToDb(AMDatabase::userdb());


	if(success) {
		emit dialogBoxClosed(newRun.id());
	}
	else {
		emit dialogBoxClosed(-1);
	}

	accept();
}

/// This function will hide the dialog box if the cancel button is pressed. Also, the dialogBoxClosed signal will be emitted
void AMNewRunDialog::cancelButtonPressed(){
	emit dialogBoxClosed(-1);
	reject();
}
