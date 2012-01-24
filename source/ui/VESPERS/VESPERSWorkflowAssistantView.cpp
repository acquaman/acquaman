#include "VESPERSWorkflowAssistantView.h"

#include <QLabel>
#include <QHBoxLayout>

VESPERSWorkflowAssistantView::VESPERSWorkflowAssistantView(VESPERSWorkflowAssistant *assistant, QWidget *parent)
    : QWidget(parent)
{
    assistant_ = assistant;

    connect(assistant_, SIGNAL(workflowRunningChanged(bool)), this, SLOT(setVisible(bool)));
}
