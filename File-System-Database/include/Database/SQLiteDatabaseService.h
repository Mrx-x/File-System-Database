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
    int upsertScanTree(const ScanItem* root, const QString& path, const QDateTime& scanTime) override;
    std::unique_ptr<ScanItem> loadScanTree(int scanId) override;
    std::unique_ptr<ScanItem> loadLastScanByPath(const QString& path) override;

    QList<int> getAvailableScans() const override;
    QList<QPair<int, QString>> getAllScanIdsAndPaths() const override;

    QString getScanPath(int scanId) const override;
    QDateTime getScanTime(int scanId) const override;

private:
    void createTablesIfNeeded();
    int insertScanMeta(int scanId, const QString& path, const QDateTime& timeStamp);
    void insertItem(const ScanItem* item, int scanId, int parentId);
    std::unique_ptr<ScanItem> loadItem(int itemId, ScanItem* parent = nullptr) const;

    int  getScanIdByPath(const QString& path) const;
    void deleteScanById(int scanId);

private:
    QSqlDatabase _database;
};

#endif // SQLITEDATABASESERVICE_H
