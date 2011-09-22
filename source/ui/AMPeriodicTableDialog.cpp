#include "AMPeriodicTableDialog.h"

#include "ui/AMPeriodicTableView.h"

#include <QHBoxLayout>

AMPeriodicTableDialog::AMPeriodicTableDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Choose an element");

	element_ = 0;

	AMPeriodicTableView *ptView = new AMPeriodicTableView;
	connect(ptView, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(ptView);

	setLayout(layout);
}

void AMPeriodicTableDialog::onElementSelected(AMElement *el)
{
	element_ = el;
	accept();
}

AMElement *AMPeriodicTableDialog::getElement(QWidget *parent)
{
	AMPeriodicTableDialog dialog(parent);

	return dialog.exec() ? dialog.element() : 0;
}

