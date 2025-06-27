#ifndef SQLITEDATABASESERVICE_H
#define SQLITEDATABASESERVICE_H

#include "Database/IDatabaseService.h"
#include <QSqlDatabase>

class SQLiteDatabaseService : public IDatabaseService
{
public:
    SQLiteDatabaseService() = default;
    ~SQLiteDatabaseService();

public:
    void open(const QString& filePath) override;
    int saveScanTree(const ScanItem* root, const QString& path, const QDateTime& scanTime) override;
    std::unique_ptr<ScanItem> loadScanTree(int scanId) override;
    QList<int> getAvailableScans() const override;
    QString getScanPath(int scanId) const override;
    QDateTime getScanTime(int scanId) const override;

private:
    void createTablesIfNeeded();
    int insertScanMeta(const QString& path, const QDateTime& timeStamp);
    void insertItem(const ScanItem* item, int scanId, int parentId);
    std::unique_ptr<ScanItem> loadItem(int itemId, ScanItem* parent = nullptr) const;

private:
    QSqlDatabase _database;
};

#endif // SQLITEDATABASESERVICE_H
