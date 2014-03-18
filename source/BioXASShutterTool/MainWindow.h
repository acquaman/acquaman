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

    void onModelOpened();
    void onModelClosed();
    void onModelBetween();

    void enableAutomaticShuttersOpenGUI(bool enabled);
    void enableShutterControlGUI(bool enabled);

    void onInjection();
    void onPostInjection();

private:
    void createComponents();
    void makeConnections();
    void defaultSettings();

    QWidget* createAppStatusDisplay();
    QWidget* createShutterDisplay();
    void turnAutomaticShutterControlOn();
    void turnAutomaticShutterControlOff();

private:
    Model *model_;
    QVBoxLayout *mainLayout_;

    QLabel *appStatusLabel_;
    QPushButton *enableAutomaticShuttersOpenButton_;
    QPushButton *disableAutomaticShuttersOpenButton_;

    QLabel *shuttersStatusLabel_;
    QPushButton *openShuttersButton_;
    QPushButton *closeShuttersButton_;
};

#endif // MAINWINDOW_H
