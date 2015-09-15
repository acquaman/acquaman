#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QGroupBox>
#include <QHBoxLayout>

class SGMTestsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SGMTestsWindow(QWidget *parent = 0);

signals:

public slots:
protected:
    QTextEdit* outputTextEdit_;
};

#endif // SGMTESTSWINDOW_H
