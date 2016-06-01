#include "CLSControlsStatusView.h"

#include "beamline/CLS/CLSShutters.h"
#include "beamline/CLS/CLSControlsStatus.h"

#include "ui/CLS/CLSControlEditor.h"

CLSControlsStatusView::CLSControlsStatusView(CLSControlsStatus *controlStatus, bool compactView, QWidget *parent) :
    QWidget(parent)
{
    // Initialize class variables.
    controlsStatus_ = 0;
    selectedComponent_ = 0;

    editor_ = 0;
    selectedComponentView_ = 0;

    compactView_ = compactView;

    // Create layouts and ui components.
    QVBoxLayout *contentLayout = new QVBoxLayout();
    setLayout(contentLayout);

    QWidget* controlsStatusWidget;
    if (compactView_){
        controlsStatusWidget = createCompactControlsStatusView();
    } else {
        controlsStatusWidget = createFullControlsStatusView();
    }
    contentLayout->addWidget(controlsStatusWidget);

    // Current settings for controls.
    setControlsStatus(controlStatus);

}

CLSControlsStatusView::~CLSControlsStatusView()
{

}

void CLSControlsStatusView::refresh()
{
    // Update the control status editor.
    if(editor_)
        editor_->setControl(controlsStatus_);

    //Update the control status button bar.
    if(componentButtonBar_)
        componentButtonBar_->setControlsStatus(controlsStatus_);

    // Update the selected component view.
    updateSelectedComponentView();
}

void CLSControlsStatusView::setControlsStatus(CLSControlsStatus *newStatus)
{
    if (controlsStatus_ != newStatus) {

            if (controlsStatus_)
                    disconnect( controlsStatus_, 0, this, 0 );

            controlsStatus_ = newStatus;

            if (controlsStatus_)
                    connect( controlsStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

            refresh();

            emit controlStatusChanged(controlsStatus_);
    }
}

void CLSControlsStatusView::setSelectedComponent(AMControl *newControl)
{
    if (selectedComponent_ != newControl && controlsStatus_->components().contains(newControl)) {
            selectedComponent_ = newControl;

            componentButtonBar_->setSelectedControl(selectedComponent_);

            refresh();

            emit selectedComponentChanged(selectedComponent_);
    }
}

void CLSControlsStatusView::updateSelectedComponentView()
{
    if (compactView_)
            return;

    // If there is an existing component view, remove it from the
    // component box layout, delete it, and hide the component box.

    if (selectedComponentView_) {
            selectedComponentBox_->layout()->removeWidget(selectedComponentView_);
            selectedComponentView_->deleteLater();
            selectedComponentView_ = 0;
            selectedComponentBox_->hide();
    }

    // Create a new view for the currently selected component. If a
    // valid view was created, add it to the component box layout
    // and show the component box.

    selectedComponentView_  = createComponentView(selectedComponent_);
    if (selectedComponentView_) {
            selectedComponentBox_->layout()->addWidget(selectedComponentView_);
            selectedComponentBox_->show();
    }

}

QWidget* CLSControlsStatusView::createCompactControlsStatusView()
{
    QLayout *componentBarLayout = createButtonBarLayout();

    QGroupBox* controlsStatusWidget = new QGroupBox("Controls status");
    controlsStatusWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    controlsStatusWidget->setLayout(componentBarLayout);

    return controlsStatusWidget;
}

QWidget* CLSControlsStatusView::createFullControlsStatusView()
{
    // Create beam status editor.
    editor_ = new CLSControlEditor(0);
    editor_->setTitle("Controls status");

    // Create components view.
    QLayout *componentBarLayout = createButtonBarLayout();

    //    --- create the selected components -----
    QLayout *selectedComponentLayout = new QVBoxLayout();

    selectedComponentBox_ = new QGroupBox();
    selectedComponentBox_->setFlat(true);
    selectedComponentBox_->setLayout(selectedComponentLayout);
    selectedComponentBox_->hide();

    QVBoxLayout *componentsBoxLayout = new QVBoxLayout();
    componentsBoxLayout->addLayout(componentBarLayout);
    componentsBoxLayout->addWidget(selectedComponentBox_);

    QGroupBox *componentsBox = new QGroupBox();
    componentsBox->setTitle("Components");
    componentsBox->setLayout(componentsBoxLayout);

    // layout the components
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->addWidget(editor_);
    contentLayout->addWidget(componentsBox);

    // the beamline status widget
    QFrame* statusWidget = new QFrame();
    statusWidget->setLayout(contentLayout);

    return statusWidget;
}

QLayout* CLSControlsStatusView::createButtonBarLayout()
{

    componentButtonBar_ = new CLSControlsStatusButtonBar(controlsStatus_);
    QHBoxLayout *componentBarLayout = new QHBoxLayout();
    componentBarLayout->addStretch();
    componentBarLayout->addWidget(componentButtonBar_);
    componentBarLayout->addStretch();

    connect( componentButtonBar_, SIGNAL(selectedControlChanged(AMControl*)), this, SLOT(setSelectedComponent(AMControl*)) );

    return componentBarLayout;

}

QWidget* CLSControlsStatusView::createComponentView(AMControl *control)
{
    QWidget *view = 0;

    if (control) {
            if (controlsStatus_->isShutterControl(control)) {
                    CLSShutters *shutters = qobject_cast<CLSShutters*>(control);
                    if (shutters) {
                            QVBoxLayout *shuttersViewLayout = new QVBoxLayout();

                            foreach (AMControl *shutter, shutters->shuttersList())
                                    shuttersViewLayout->addWidget(new CLSControlEditor(shutter));

                            view = new QWidget();
                            view->setLayout(shuttersViewLayout);
                    }
            } else {
                    QString title = "";
                    if ( controlsStatus_->isValveControl(control) ) {
                            title = "Valves";
                    } else if ( controlsStatus_->isMirrorMaskControl(control) ) {
                            title = "Mirror Mask";
                    } else if ( controlsStatus_->isMonoMaskControl(control) ) {
                            title = "Monochromator Mask";
                    }

                    if (title.length() > 0) {
                            CLSControlEditor *editor = new CLSControlEditor(control);
                            editor->setTitle(title);
                            view = editor;
                    }
            }
    }

    return view;
}

