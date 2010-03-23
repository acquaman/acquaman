#include "Database.h"

#include <QDebug>

Database* Database::instance_ = 0;

// This constructor is protected; only access is through Database::db()
Database::Database() : QObject() {

    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(UserSettings::userDataFolder + UserSettings::userDatabaseFilename);
    bool ok = db_.open();

    if(ok) {
        qDebug() << "Database: connection established to Sqlite3";
    }
    else {
        // todo: error handling
        qDebug() << "Database: error connecting to database";
    }
}

Database* Database::db() {
    if(instance_ == 0)
        instance_ = new Database();
    return instance_;
}

void Database::releaseDb() {
    if(instance_) {
        delete instance_;
        instance_ = 0;
    }
}
