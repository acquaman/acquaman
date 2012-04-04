#ifndef AMADDACTIONDIALOG3_H
#define AMADDACTIONDIALOG3_H

#include <QDialog>

namespace Ui {
    class AMAddActionDialog3;
}

class QListWidgetItem;

class AMAddActionDialog3 : public QDialog
{
    Q_OBJECT

public:
	/// Constructor. The view is populated during construction with all actions currently registered in the AMActionRegistry [ActionInfo/Action].
    explicit AMAddActionDialog3(QWidget *parent = 0);

    ~AMAddActionDialog3();


protected slots:
	/// Called when the selected item is changed in the list view; this updates the item-specific title and description shown on the right of the dialog, and enables the 'Add to Workflow' button.
	void onListWidgetCurrentIndexChanged(QListWidgetItem* current, QListWidgetItem* previous);

	/// Called when the workflow button is clicked. Attempts to instantiate the action based on a default-constructed AMActionInfo, and add it to the workflow.
	void onAddToWorkflowButtonClicked();

protected:
	/// Helper function to clear the existing actions displayed in the list view, and populate it with all the actions registered in AMActionRegistry.
	void populateWithRegisteredActions();

	/// UI elements
    Ui::AMAddActionDialog3 *ui;
};

#endif // AMADDACTIONDIALOG_H
