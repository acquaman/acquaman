#ifndef VESPERSWORKFLOWASSISTANT_H
#define VESPERSWORKFLOWASSISTANT_H

#include <QObject>

#include "ui/AMWorkflowManagerView.h"

/*!
  This class acts as an assistant to the current version of the workflow to allow for multiple copies of a scan to be repeated.  The idea
  is that the number of scans can be quickly and easily changed, even if the scan is being run.  It listens to the workflow
  and adds scan actions based on the current scan and the total number defined in the scan.  The primary goal of this class is to
  provide the same seamless control of the workflow that is currently available while adding the ability for repeatable scans.  It should not
  hinder any of the current useability of the workflow, even if currently the workflow is hardly utilized with VESPERS at present.
  */
class VESPERSWorkflowAssistant : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes a pointer to the workflow.
	explicit VESPERSWorkflowAssistant(AMWorkflowManagerView *workflow, QObject *parent = 0);

signals:

public slots:

protected slots:
	/// Slot that listens to the workflow and updates internal pieces of this class.
	void listen();

protected:
	/// Pointer to the workflow.
	AMWorkflowManagerView *workflow_;

};

#endif // VESPERSWORKFLOWASSISTANT_H
