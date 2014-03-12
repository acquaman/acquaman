#ifndef RELOADPVDIALOG_H
#define RELOADPVDIALOG_H

#include <QDialog>
#include <QtGui>

class ReloadPVDialog : public QDialog
{
    Q_OBJECT
public:
 	virtual ~ReloadPVDialog();
    explicit ReloadPVDialog(QList<QString> reloadList, QWidget *parent = 0);

public:
    QStringList pvNamesList();

protected:
    QStringList pvNamesList_;

protected slots:
    void onItemChanged(QStandardItem *item);

};

#endif // RELOADPVDIALOG_H
