#ifndef BIOXASMAINMONOCRYSTALCHANGEVIEW_H
#define BIOXASMAINMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>
#include <QPushButton>

#include "beamline/BioXAS/BioXASMainMonochromator.h"

class BioXASMainMonoCrystalChangeViewMessage;
class BioXASMainMonoCrystalChangeViewActions;

class BioXASMainMonoCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonoCrystalChangeView(BioXASMainMonochromator *mono, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonoCrystalChangeView();

signals:
    /// Notifier that the mono being viewed has changed.
    void monoChanged(BioXASMainMonochromator *newMono);

public slots:
    /// Sets the mono being viewed.
    void setMono(BioXASMainMonochromator *newMono);

protected slots:
    /// Displays the start view, and hides the others.
    void showStartView();
    /// Displays the actions view, and hides the others.
    void showActionsView();
    /// Displays the final view, and hides the others.
    void showFinalView();
    /// Handles changes to the view as a result of changes to mono connection state.
    void onMonoConnectedChanged(bool isConnected);
    /// Handles changes to the view as a result of the actions' completion.
    void onActionsComplete(bool successful);

protected:
    /// The mono being viewed.
    BioXASMainMonochromator *mono_;
    /// The view used to display messages.
    BioXASMainMonoCrystalChangeViewMessage *startView_;
    /// The view for all steps.
    BioXASMainMonoCrystalChangeViewActions *actionsView_;
    /// The final view, displayed when steps are complete or upon error.
    BioXASMainMonoCrystalChangeViewMessage *finalView_;
    /// String containing the crystal change prompt.
    QString promptMessage_;
    /// String containing the success message to be displayed in the final view.
    QString successMessage_;
    /// String containing the failure message to be displayed in the final view.
    QString failMessage_;

private:
    /// Starts the crystal change actions.
    bool startCrystalChange();
    /// Cancels the crystal change actions.
    bool cancelCrystalChange();
};

//

class BioXASMainMonoCrystalChangeViewMessage : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonoCrystalChangeViewMessage(QWidget *parent = 0, const QString &message = "", bool showOKButton = true, bool showCloseButton = false);
    /// Destructor.
    virtual ~BioXASMainMonoCrystalChangeViewMessage();

signals:
    /// Notifier that the ok button has been clicked.
    void okClicked();
    /// Notifier that the close button has been clicked.
    void closeClicked();

public slots:
    /// Sets the message displayed.
    void setMessage(const QString &newMessage) { message_->setText(newMessage); }
    /// Sets whether the ok button is shown.
    void setOKButtonShown(bool isShown) { okButton_->setVisible(isShown); }
    /// Sets whether the ok button is enabled.
    void setOKButtonEnabled(bool isEnabled) { okButton_->setEnabled(isEnabled); }
    /// Sets whether the ok button is the default button.
    void setOKButtonDefault(bool isDefault) { okButton_->setDefault(isDefault); }
    /// Sets whether the close button is shown.
    void setCloseButtonShown(bool isShown) { closeButton_->setVisible(isShown); }
    /// Sets whether the close button is enabled.
    void setCloseButtonEnabled(bool isEnabled) { closeButton_->setEnabled(isEnabled); }
    /// Sets whether the close button is the default button.
    void setCloseButtonDefault(bool isDefault) { closeButton_->setDefault(isDefault); }

protected:
    /// The message displayed.
    QLabel *message_;
    /// The ok button.
    QPushButton *okButton_;
    /// The close button.
    QPushButton *closeButton_;
};

//

class BioXASMainMonoCrystalChangeViewAction;

class BioXASMainMonoCrystalChangeViewActions : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonoCrystalChangeViewActions(QWidget *parent = 0, AMAction3 *actions = 0);
    /// Destructor.
    virtual ~BioXASMainMonoCrystalChangeViewActions();
    /// Returns the actions being viewed.
    AMAction3* actions() const { return actions_; }

signals:
    /// Notifier that the actions being viewed have changed.
    void actionsChanged(AMAction3 *newActions);
    /// Notifier that the actions are complete.
    void actionsComplete(bool successful);

public slots:
    /// Sets the actions to be viewed.
    void setActions(AMAction3 *newActions);

protected slots:
    /// Updates the view to display information on the currently executing action.
    void onCurrentSubActionChanged(int newIndex);
    /// Updates the progress bar with actions progress.
    void onActionsProgressChanged(double num, double denom);
    /// Emits the actionsComplete signal with argument true.
    void onActionsSucceeded() { emit actionsComplete(true); }
    /// Emits the actionsComplete signal with argument false, when actions are cancelled.
    void onActionsCancelled() { emit actionsComplete(false); }
    /// Emits the actionsComplete signal with argument false, when actions have failed.
    void onActionsFailed() { emit actionsComplete(false); }
    
protected:
    /// The actions being viewed.
    AMListAction3 *actions_;
    /// The currently displayed action view.
    BioXASMainMonoCrystalChangeViewAction *actionView_;
    /// Indicates progress as steps are executed.
    QProgressBar *progressBar_;
};

//

class BioXASMainMonoCrystalChangeViewAction : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonoCrystalChangeViewAction(AMAction3 *action, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonoCrystalChangeViewAction();

signals:
    /// Notifier that the action being viewed has changed.
    void actionChanged(AMAction3 *newAction);

public slots:
    /// Clears the view.
    void clear();
    /// Updates the view.
    void update();
    /// Sets the action being viewed.
    void setAction(AMAction3 *newAction);

protected:
    /// The action being viewed.
    AMAction3 *action_;
    /// The label containing the action description.
    QLabel *description_;
    /// The label containing the action instructions (if any).
    QLabel *instructions_;
};

#endif // BIOXASMAINMONOCRYSTALCHANGEVIEW_H
