#ifndef ENTRYWIDGET_H
#define ENTRYWIDGET_H

#include <QWidget>
#include <QtGui>

class EntryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EntryWidget(QWidget *parent = 0);
    ~EntryWidget();

signals:
    void reset();
    void entryComplete(const QString &textEntered);

public:

public slots:
    void onAddButtonClicked();
    void toTestSignal(const QString &textEntered);

private:
    void buildComponents();
    void makeConnections();
    void defaultSettings();

private:
    QLineEdit *lineEdit_;
    QPushButton *addButton_;

};

#endif // ENTRYWIDGET_H
