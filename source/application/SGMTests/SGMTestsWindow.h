#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class SGMEnergyTrajectoryTestView;
class SGMMonochromatorInfoTestView;
/*!
 * A class which represents the main window of the SGM tests application.
 */
class SGMTestsWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*!
     * Creates an instance of an sgm tests main window.
     */
    explicit SGMTestsWindow(QWidget *parent = 0);

signals:

public slots:
protected slots:
    /*!
     * Handles the show mono view button being clicked.
     */
    void onShowMonoViewClicked();

    /*!
     * Handles the show trajectory view button being clicked.
     */
    void onShowTrajectoryViewClicked();
protected:

    /*!
     * Helper function which initializes the child widget components.
     */
    void setupUi();
    QTextEdit* outputTextEdit_;
    QPushButton* showMonoViewButton_;
    QPushButton* showTrajectoryViewButton_;

    SGMEnergyTrajectoryTestView* energyTestView_;
    SGMMonochromatorInfoTestView* monoInfoTestView_;
};

#endif // SGMTESTSWINDOW_H
