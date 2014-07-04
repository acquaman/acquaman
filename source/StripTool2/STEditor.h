#ifndef STEDITOR_H
#define STEDITOR_H

#include <QWidget>

class STEditor : public QWidget
{
    Q_OBJECT
public:
    explicit STEditor(QWidget *parent = 0);
    virtual ~STEditor();

signals:

public slots:
    virtual void applyChanges() = 0;

};

#endif // STEDITOR_H
