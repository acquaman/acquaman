#ifndef AMVALIDATOR_H
#define AMVALIDATOR_H

#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
/*!
  * A simple class for recording the valid state of an object.
  */
class AMValidator : public QObject
{
    Q_OBJECT
public:
    /*!
      * Creates a new instance of an AM Validator which is initialized in the valid
      * state.
      */
    explicit AMValidator(QObject *parent = 0);

    /*!
      * Virtual destructor for an AM Validator
      */
    virtual ~AMValidator() { }

    /*!
      * Updates the record of validity failures based on the passesTest value.
      *   If passesTest is true the given failMessage is removed from the validator.
      *   If passesTest is false then given failMessage is ensured to exist in the
      *   validator.
      * \param failMessage ~ The failure message uniquely identified with a failure
      * to validate.
      * \param failureCriteria ~ A boolean which indicates that the validity test
      * has failed.
      */
    void updateValidity(const QString& failMessage, bool failureCriteria);

    /*!
      * Whether the validator currently has the object being validated in a valid
      * state.
      */
    bool isValid() const;

    /*!
      * The number of failures the validator has recorded for the object being
      * validatated.
      */
    int failCount() const;

    /*!
      * A list containing all the messages which are preventing the object being
      * validated from being in a valid state.
      */
    QStringList failureMessages() const;

    /*!
     * A string containing all the error messages associated with the object,
     * sepearated by the optional delimiter.
     * \param delimiter ~ An optional delimiter used to separate each message. If
     * none is provided then a newline is used
     * \return
     */
    QString fullFailureMessage(const QString& delimiter = "\n") const;
signals:

    /*!
      * Signal indicating that the overall valid state of the object being validated
      * has altered.
      * \param validState ~ The new valid state of the object.
      */
    void validStateChanged(bool validState);

    /*!
      * Signal indicating that the number of validation failures has altered.
      * \param failCount ~ The new number of failures.
      */
    void failCountChanged(int failCount);

    /*!
     * Signal indicating that a failure has been removed.
     * \param failureMessage ~ A string associated with the failure which was
     * removed.
     */
    void failureRemoved(const QString& failureMessage);

    /*!
     * Signal indicating that a failure has been added to the validator.
     * \param failureAdded ~ A string associated with the failure which was
     * added.
     */
    void failureAdded(const QString& failureAdded);
public slots:

protected:
    QSet<QString> failMessages_;
};

#endif // AMVALIDATOR_H
