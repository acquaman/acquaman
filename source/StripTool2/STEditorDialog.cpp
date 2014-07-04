#include "STEditorDialog.h"

STEditorDialog::STEditorDialog(STEditor *editor, QWidget *parent) : QWidget(parent)
{
    QWidget::setWindowFlags(Qt::Window);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    editor_ = editor;
    if (editor_) {
        editor_->setParent(this);
        mainLayout->addWidget(editor_);
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addStretch();

    okButton_ = new QPushButton("Ok", this);
    okButton_->setDefault(true);
    connect( okButton_, SIGNAL(clicked()), this, SLOT(onOkButtonClicked()) );
    buttonLayout->addWidget(okButton_);

    cancelButton_ = new QPushButton("Cancel", this);
    connect( cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()) );
    buttonLayout->addWidget(cancelButton_);

    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);

    qDebug() << "Dialog created.";
}

STEditorDialog::~STEditorDialog()
{

}

STEditor* STEditorDialog::editor() const
{
    return editor_;
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
