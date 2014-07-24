#include "STEditorDialog.h"

STEditorDialog::STEditorDialog(STEditor *editor, QWidget *parent) : QWidget(parent)
{
    // Set variables.

    editor_ = editor;
    okButton_ = new QPushButton("Ok", this);
    cancelButton_ = new QPushButton("Cancel", this);

    // Create UI.

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    if (editor_)
        mainLayout->addWidget(editor_);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // Get current settings.

    okButton_->setDefault(true);
    QWidget::setWindowFlags(Qt::Window);

    // Make connections.

    connectEditor();

    connect( okButton_, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()) );
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()) );
}

STEditorDialog::~STEditorDialog()
{

}

STEditor* STEditorDialog::editor() const
{
    return editor_;
}

void STEditorDialog::setEditor(STEditor *editor)
{
    if (editor_) {
        disconnectEditor();
        editor_ = 0;
    }

    if (editor_ != editor) {
        editor_ = editor;
        editor_->setParent(this);
        connectEditor();

        emit editorChanged(editor_);

        update();

    } else {
        emit editorChanged(0);
    }
}

void STEditorDialog::onEditorCloseDialog()
{
    close();
}

void STEditorDialog::onOkButtonClicked()
{
    if (editor_)
        editor_->applyChanges();

    close();
}

void STEditorDialog::onCancelButtonClicked()
{
    close();
}

void STEditorDialog::connectEditor()
{
    if (editor_) {
        connect( editor_, SIGNAL(closeDialog()), this, SLOT(onEditorCloseDialog()) );
    }
}

void STEditorDialog::disconnectEditor()
{
    if (editor_) {
        disconnect( editor_, SIGNAL(closeDialog()), this, SLOT(onEditorCloseDialog()) );
    }
}
