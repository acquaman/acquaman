#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPushButton>

class SGMTestsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SGMTestsWindow(QWidget *parent = 0);

signals:

public slots:
protected slots:
    void onShowMonoViewClicked();
protected:
    QTextEdit* outputTextEdit_;
    QPushButton* showMonoViewButton_;
};

#endif // SGMTESTSWINDOW_H
