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

public slots:
    void onOkButtonClicked();
    void onCancelButtonClicked();

protected:
    STEditor *editor_;

    QPushButton *okButton_;
    QPushButton *cancelButton_;

};

#endif // STEDITORDIALOG_H
