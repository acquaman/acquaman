#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "actions3/AMAction3.h"

#include "Model.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

protected:
    Model* model();

protected slots:
    void onEnableButtonClicked();
    void onDisableButtonClicked();
    void onOpenButtonClicked();
    void onCloseButtonClicked();

    void onModelConnected(bool isConnected);
    void onModelOpened();
    void onModelClosed();
    void onModelBetween();

private:
    void createComponents();
    QWidget* createAppStatusDisplay();
    QWidget* createShutterDisplay();
    void makeConnections();
    void turnAutomaticShutterControlOn();
    void turnAutomaticShutterControlOff();

private:
    Model *model_;
    QVBoxLayout *mainLayout_;

    QLabel *appStatusLabel_;
    QPushButton *enableButton_;
    QPushButton *disableButton_;

    QLabel *shuttersStatus_;
    QPushButton *openButton_;
    QPushButton *closeButton_;
};

#endif // MAINWINDOW_H
