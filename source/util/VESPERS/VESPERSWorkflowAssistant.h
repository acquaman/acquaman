#ifndef VESPERSWORKFLOWASSISTANT_H
#define VESPERSWORKFLOWASSISTANT_H

#include <QObject>

#include "ui/AMWorkflowManagerView.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"

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

    /// Returns the name of the current scan configuration.  Returns 0 if there is no scan to be viewed.
    /*! \note Currently returns only VESPERSEXAFSScanConfiguration because it is the only type of scan currently supported.
        If more scans become supported before the workfow is rewritten then this class will be extended.
        */
    VESPERSEXAFSScanConfiguration *currentConfiguration() const { return config_; }

    /// Returns the name of the current scan configuration.
    QString configName() const { return config_ != 0 ? config_->name() : ""; }
    /// Returns the current total number of scans that should be executed.  Returns -1 if the the configuration is invalid.
    int totalScans() const { return config_ != 0 ? config_->numberOfScans() : -1; }
    /// Returns the current scan running.  Returns -1 if no scans are running.
    int currentScan() const { return currentScan_; }

signals:
    /// Notifier that the workflow is running or not.
    void workflowRunningChanged(bool);

public slots:

protected slots:
	/// Slot that listens to the workflow and updates internal pieces of this class.
	void listen();

protected:
	/// Pointer to the workflow.
	AMWorkflowManagerView *workflow_;

    /// Current number of scans that have been scanned in the current set.
    int currentScan_;

    /// The current scan configuration.
    VESPERSEXAFSScanConfiguration *config_;

};

#endif // VESPERSWORKFLOWASSISTANT_H
