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
    void entryEnabled(bool isEnabled);
    void clearEntry();
    void entryComplete(const QString &textEntered);

protected:
    QLineEdit *lineEdit_;
    QPushButton *button_;
    QString textEntered_;

public:
    void setLineEditText(const QString &text);
    void setButtonText(const QString &text);
    void setWidthMax(int maxWidth);
    void setWidthMin(int minWidth);

protected slots:
    void toSaveEntry(const QString &newText);
    void addClicked();
    void toTestSignal(const QString &textEntered);

};

#endif // ENTRYWIDGET_H
