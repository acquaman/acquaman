#ifndef CLSCONTROLSSTATUSVIEW_H
#define CLSCONTROLSSTATUSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLayout>

#include "ui/CLS/CLSControlsStatusButtonBar.h"

class AMControl;
class CLSControlsStatus;
class CLSControlEditor;

class CLSControlsStatusView : public QWidget
{
    Q_OBJECT

public:

    /// Constructor
    explicit CLSControlsStatusView(CLSControlsStatus *beamlinestatus, bool compactView, QWidget *parent = 0);

    ///Destructor
    virtual ~CLSControlsStatusView();

    /// Returns the beam status being viewed.
    CLSControlsStatus* controlStatus() const { return controlsStatus_; }
    /// Returns the selected component.
    AMControl* selectedComponent() const { return selectedComponent_; }

signals:
    /// Notifier that the beam status being viewed has changed.
    void controlStatusChanged(CLSControlsStatus *newStatus);
    /// Notifier that hte selected control in the controls status buttons view has changed.
    void selectedComponentChanged(AMControl *control);

public slots:
    /// Refreshes the view.
    void refresh();

    void setControlsStatus(CLSControlsStatus *newStatus);
    void setSelectedComponent(AMControl *newControl);

protected slots:
    /// Updates the
    void updateSelectedComponentView();

protected:
    /// Creates and lays out the compact controls status view
    QWidget* createCompactControlsStatusView();
    QWidget* createFullControlsStatusView();
    QLayout* createButtonBarLayout();
    virtual QWidget* createComponentView(AMControl *control);

protected:

    bool compactView_;

    CLSControlsStatus *controlsStatus_;

    AMControl *selectedComponent_;

    CLSControlEditor *editor_;

    CLSControlsStatusButtonBar *componentButtonBar_;

    QWidget *selectedComponentView_;

    QGroupBox *selectedComponentBox_;

};

#endif // CLSCONTROLSSTATUSVIEW_H
