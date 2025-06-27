#include "Database/SQLiteDatabaseService.h"
#include "Model/ScanItem.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

SQLiteDatabaseService::~SQLiteDatabaseService()
{
    if (_database.isOpen())
    {
        _database.close();
    }
}

void SQLiteDatabaseService::open(const QString &filePath)
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName(filePath);
    if (!_database.open())
    {
        qFatal("Cannot open Database: %s", qPrintable(_database.lastError().text()));
    }
    createTablesIfNeeded();
}

int SQLiteDatabaseService::saveScanTree(const ScanItem *root, const QString &path, const QDateTime &scanTime)
{
    _database.transaction();

    int scanId = insertScanMeta(path, scanTime);
    insertItem(root, scanId, -1);

    _database.commit();
    return scanId;
}

std::unique_ptr<ScanItem> SQLiteDatabaseService::loadScanTree(int scanId)
{
    QSqlQuery query(_database);
    query.prepare("SELECT id FROM scan_items WHERE scan_id=:sid AND parent_id IS NULL");
    query.bindValue(":sid", scanId);
    query.exec();

    if (!query.next())
    {
        return nullptr;
    }

    return loadItem(query.value(0).toInt(), nullptr);
}

QList<int> SQLiteDatabaseService::getAvailableScans() const
{
    QList<int> list;
    QSqlQuery query(_database);

    query.exec("SELECT id FROM scans ORDER BY id");
    while (query.next())
    {
        list << query.value(0).toInt();
    }
    return list;
}

QString SQLiteDatabaseService::getScanPath(int scanId) const
{
    QSqlQuery query(_database);
    query.prepare("SELECT path FROM scans WHERE id=:id");

    query.bindValue(":id", scanId);
    query.exec();
    return query.next() ? query.value(0).toString() : QString();
}

QDateTime SQLiteDatabaseService::getScanTime(int scanId) const
{
    QSqlQuery query(_database);
    query.prepare("SELECT scan_time FROM scans WHERE id=:id");

    query.bindValue(":id", scanId);
    query.exec();
    return query.next() ? QDateTime::fromString(query.value(0).toString(), Qt::ISODate)
                        : QDateTime();
}

void SQLiteDatabaseService::createTablesIfNeeded()
{
    QSqlQuery query(_database);
    query.exec(R"(
        CREATE TABLE IF NOT EXISTS scans (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        path TEXT NOT NULL,
        scan_time TEXT NOT NULL
      );
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS scan_items (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        scan_id INTEGER NOT NULL,
        parent_id INTEGER,
        name TEXT NOT NULL,
        size INTEGER NOT NULL,
        type INTEGER NOT NULL,
        FOREIGN KEY(scan_id) REFERENCES scans(id),
        FOREIGN KEY(parent_id) REFERENCES scan_items(id)
      );
    )");
}

int SQLiteDatabaseService::insertScanMeta(const QString &path, const QDateTime &timeStamp)
{
    QSqlQuery query(_database);
    query.prepare("INSERT INTO scans(path, scan_time) VALUES(:p, :t)");

    query.bindValue(":p", path);
    query.bindValue(":t", timeStamp);

    query.exec();
    return query.lastInsertId().toInt();
}

void SQLiteDatabaseService::insertItem(const ScanItem *item, int scanId, int parentId)
{
    QSqlQuery query(_database);
    query.prepare(R"(
      INSERT INTO scan_items (scan_id, parent_id, name, size, type)
      VALUES(:sid, :pid, :n, :s, :t)
    )");

    query.bindValue(":sid", scanId);
    if (parentId < 0)
    {
        query.bindValue(":pid", QVariant(QVariant::Int));
    }
    else
    {
        query.bindValue(":pid", parentId);
    }

    query.bindValue(":n", item->name());
    query.bindValue(":s", item->size());
    query.bindValue(":t", static_cast<int>(item->type()));

    query.exec();

    int itemId = query.lastInsertId().toInt();
    for (int i = 0; i < item->childCount(); ++i)
    {
        insertItem(item->child(i), scanId, itemId);
    }
}

std::unique_ptr<ScanItem> SQLiteDatabaseService::loadItem(int itemId, ScanItem *parent) const
{
    QSqlQuery query(_database);
    query.prepare("SELECT name, size, type FROM scan_items WHERE id=:id");
    query.bindValue(":id", itemId);
    query.exec();

    if (!query.next()) return nullptr;

    QString name = query.value(0).toString();
    qint64 size = query.value(1).toLongLong();
    auto type = static_cast<ScanItem::Type>(query.value(2).toInt());
    auto node = std::make_unique<ScanItem>(name, size, type, parent);

    QSqlQuery queryForChild(_database);
    queryForChild.prepare("SELECT id FROM scan_items WHERE parent_id=:pid");

    queryForChild.bindValue(":pid", itemId);
    queryForChild.exec();

    while (queryForChild.next())
    {
        int cid = queryForChild.value(0).toInt();
        auto ch = loadItem(cid, node.get());
        node->appendChild(std::move(ch));
    }

    return node;
}
