#ifndef VESPERSWORKFLOWASSISTANTVIEW_H
#define VESPERSWORKFLOWASSISTANTVIEW_H

#include <QWidget>
#include <QProgressBar>
#include <QSpinBox>

#include "util/VESPERS/VESPERSWorkflowAssistant.h"

/*! This class builds a view from a VESPERSWorkflowAssistant.  It behaves similarly to the bottom bar in that it offers some
  scan controlling aspects.  It will contain the number of scans, a progress bar for the overall number of scans progress,
  and also provide some other rudimentary information about the scan currently scanning, such as the name.  If more information
  is required then it will also be added.

  Unless it is incredibly tacky, this view will only be visible when XANES/EXAFS scans are running.
  */
class VESPERSWorkflowAssistantView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.  Builds the view around the VESPERSWorkflowAssistant object passed into it.
    explicit VESPERSWorkflowAssistantView(VESPERSWorkflowAssistant *assistant, QWidget *parent = 0);
    
signals:
    
public slots:
    
protected:
    /// Pointer to the workflow assistant.
    VESPERSWorkflowAssistant *assistant_;
    /// The progress bar of all of the set of scans.
    QProgressBar *progressBar_;
    /// The total number of scans.
    QSpinBox *totalScans_;
};

#endif // VESPERSWORKFLOWASSISTANTVIEW_H
