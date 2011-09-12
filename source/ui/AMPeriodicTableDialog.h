#ifndef AMPERIODICTABLEDIALOG_H
#define AMPERIODICTABLEDIALOG_H

#include <QDialog>

#include "util/AMElement.h"

/*! This class encapsulates the AMPeriodicTableView inside a QDialog.  The purpose is to create a modal dialog that passes the element that was clicked
	to the calling class.  Essentially, by clicking on one of the element tool buttons, that calls the accept() signal and passes that element.
*/
class AMPeriodicTableDialog : public QDialog
{
	Q_OBJECT
public:
	/// Constructor.  Builds the dialog to look exactly like AMPeriodicTableView.
	explicit AMPeriodicTableDialog(QWidget *parent = 0);

	/// Static member.  Builds a dialog and returns an AMElement *.  If the dialog is cancelled then the dialog returns 0.
	static AMElement *getElement(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles the element passing from the periodic table view.
	void onElementSelected(AMElement *el);

protected:
	/// Returns the current state of element pointed to by this dialog.
	AMElement *element() const { return element_; }

	/// Pointer to the element this dialog is meant to provide.  Defaults to 0.
	AMElement *element_;
};

#endif // AMPERIODICTABLEDIALOG_H
