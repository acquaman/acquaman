#ifndef STEDITOR_H
#define STEDITOR_H

#include <QWidget>
#include <QFrame>

class STEditor : public QFrame
{
    Q_OBJECT
public:
    explicit STEditor(QWidget *parent = 0);
    virtual ~STEditor();

signals:
    void closeDialog();

public slots:
    virtual void applyChanges() = 0;

};

#endif // STEDITOR_H
