#ifndef SGMTESTSWINDOW_H
#define SGMTESTSWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class SGMEnergyTrajectoryTestView;
class SGMEnergyPositionTestView;
class SGMEnergyControlTestView;
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
	  * Handles the show energy view button being clicked.
	  */
    void onShowEnergyViewClicked();

    /*!
     * Handles the show trajectory view button being clicked.
     */
    void onShowTrajectoryViewClicked();

	/*!
	  * Handles the show energy control view being clicked.
	  */
    void onShowEnergyControlViewClicked();
protected:

    /*!
     * Helper function which initializes the child widget components.
	 */
    void setupUi();
    QTextEdit* outputTextEdit_;
    QPushButton* showEnergyViewButton_;
    QPushButton* showTrajectoryViewButton_;
    QPushButton* showEnergyControlViewButton_;

    SGMEnergyTrajectoryTestView* energyTrajectoryTestView_;
    SGMEnergyPositionTestView* energyTestView_;
    SGMEnergyControlTestView* energyControlTestView_;
};

#endif // SGMTESTSWINDOW_H
