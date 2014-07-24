#ifndef STEDITORDIALOG_H
#define STEDITORDIALOG_H

#include <QWidget>
#include <QLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QDebug>

#include "STEditor.h"

class STEditorDialog : public QWidget
{
    Q_OBJECT

public:
    explicit STEditorDialog(STEditor *editor, QWidget *parent = 0);
    virtual ~STEditorDialog();

    STEditor *editor() const;

signals:
    void editorChanged(STEditor *editor);

public slots:
    void setEditor(STEditor *editor);

protected slots:
    void onEditorCloseDialog();
    void onOkButtonClicked();
    void onCancelButtonClicked();

protected:
    void connectEditor();
    void disconnectEditor();

protected:
    STEditor *editor_;

    QPushButton *okButton_;
    QPushButton *cancelButton_;

};

#endif // STEDITORDIALOG_H
