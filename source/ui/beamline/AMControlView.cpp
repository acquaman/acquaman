#include "AMControlView.h"

AMControlView::AMControlView(AMControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	nameLabel_ = new QLineEdit();
	nameLabel_->setReadOnly(true);
	nameLabel_->setAlignment(Qt::AlignCenter);

	descriptionLabel_ = new QLineEdit();
	descriptionLabel_->setReadOnly(true);
	descriptionLabel_->setAlignment(Qt::AlignCenter);

	connectedLabel_ = new QLineEdit();
	connectedLabel_->setReadOnly(true);
	connectedLabel_->setAlignment(Qt::AlignCenter);

	valueLabel_ = new QLineEdit();
	valueLabel_->setReadOnly(true);
	valueLabel_->setAlignment(Qt::AlignCenter);

	valueMinimumLabel_ = new QLineEdit();
	valueMinimumLabel_->setReadOnly(true);
	valueMinimumLabel_->setAlignment(Qt::AlignCenter);

	valueMaximumLabel_ = new QLineEdit();
	valueMaximumLabel_->setReadOnly(true);
	valueMaximumLabel_->setAlignment(Qt::AlignCenter);

	valuesPrompt_ = new QLabel("Values: ");

	valuesView_ = new QTextEdit();
	valuesView_->setReadOnly(true);

	// Create and set layouts.

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(new QLabel("Name: "), 0, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(nameLabel_, 0, 1);
	layout->addWidget(new QLabel("Description: "), 1, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(descriptionLabel_, 1, 1);
	layout->addWidget(new QLabel("Connected: "), 2, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(connectedLabel_, 2, 1);
	layout->addWidget(new QLabel("Value: "), 3, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(valueLabel_, 3, 1);
	layout->addWidget(new QLabel("Minimum: "), 4, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(valueMinimumLabel_, 4, 1);
	layout->addWidget(new QLabel("Maximum: "), 5, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(valueMaximumLabel_, 5, 1);
	layout->addWidget(valuesPrompt_, 6, 0, 1, 1, Qt::AlignRight);
	layout->addWidget(valuesView_, 6, 1);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

AMControlView::~AMControlView()
{

}

void AMControlView::refresh()
{
	updateNameLabel();
	updateDescriptionLabel();
	updateConnectedLabel();
	updateValueLabel();
	updateMinimumLabel();
	updateMaximumLabel();
	updateValuesView();
}

void AMControlView::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(updateConnectedLabel()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateValueLabel()) );
			connect( control_, SIGNAL(enumChanged()), this, SLOT(updateValuesView()) );
			connect( control_, SIGNAL(minimumValueChanged(double)), this, SLOT(updateMinimumLabel()) );
			connect( control_, SIGNAL(maximumValueChanged(double)), this, SLOT(updateMaximumLabel()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMControlView::updateNameLabel()
{
	QString name = "";

	if (control_)
		name = control_->name();

	nameLabel_->setText(name);
}

void AMControlView::updateDescriptionLabel()
{
	QString description = "";

	if (control_)
		description = control_->description();

	descriptionLabel_->setText(description);
}

void AMControlView::updateConnectedLabel()
{
	QString connected = "";

	if (control_)
		connected = (control_->isConnected() ? "Yes" : "No");

	connectedLabel_->setText(connected);
}

void AMControlView::updateValueLabel()
{
	QString value = "";

	if (control_) {
		value = QString("%1").arg(control_->value());

		if (!control_->units().isEmpty())
			value.append(QString(" %1").arg(control_->units()));
	}

	valueLabel_->setText(value);
}

void AMControlView::updateMinimumLabel()
{
	QString minimum = "";

	if (control_) {
		minimum = QString("%1").arg(control_->minimumValue());

		if (!control_->units().isEmpty())
			minimum.append(QString(" %1").arg(control_->units()));
	}

	valueMinimumLabel_->setText(minimum);
}

void AMControlView::updateMaximumLabel()
{
	QString maximum = "";

	if (control_) {
		maximum = QString("%1").arg(control_->maximumValue());

		if (!control_->units().isEmpty())
			maximum.append(QString(" %1").arg(control_->units()));
	}

	valueMaximumLabel_->setText(maximum);
}

void AMControlView::updateValuesView()
{
	QString values = "";

	if (control_) {

		QStringList enumNames = control_->enumNames();

		for (int enumIndex = 0, enumCount = enumNames.count(); enumIndex < enumCount; enumIndex++) {
			QString enumName = enumNames.at(enumIndex);

			if (!enumName.isEmpty())
				values.append(QString("%1 - %2").arg(enumIndex).arg(enumName));
			else
				values.append(QString("[Empty enum name]"));

			if (enumIndex < enumCount - 1)
				values.append("\n");
		}
	}

	if (!values.isEmpty()) {
		valuesPrompt_->show();
		valuesView_->show();
		valuesView_->setText(values);
	} else {
		valuesPrompt_->hide();
		valuesView_->hide();
	}
}






AMControlViewDialog::AMControlViewDialog(AMControl *control, QWidget *parent, Qt::WindowFlags flags) :
	QDialog(parent, flags)
{
	// Create UI elements.

	AMControlView *controlView = new AMControlView(control, this);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(controlView);
	layout->addWidget(buttonBox);

	setLayout(layout);

	// Make connections.

	connect( buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
}

AMControlViewDialog::~AMControlViewDialog()
{

}
