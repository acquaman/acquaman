#ifndef DATABASE_H
#define DATABASE_H

#include "Settings.h"
#include <QObject>
#include <QSqlDatabase>

/*! This class encapsulates all access to the user's metadata.  It is a singleton class like Beamline.  All access is through Database::db().
  */

class Database : QObject {
    Q_OBJECT

public:
    /// Get instance of singleton database class.
    static Database* db();
    /// Free the instance if no longer needed
    static void releaseDb();

protected:
    QSqlDatabase db_;

private:
    Database();
    static Database* instance_;
};

#endif // DATABASE_H
