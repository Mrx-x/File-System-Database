#ifndef IDATABASESERVICE_H
#define IDATABASESERVICE_H

#include <QString>
#include <QDateTime>

class ScanItem;

class IDatabaseService
{
public:
    virtual ~IDatabaseService() = default;

public:
    virtual void open(const QString& path) = 0;
    virtual int upsertScanTree(const ScanItem* root, const QString& path, const QDateTime& scanTime) = 0;
    virtual std::unique_ptr<ScanItem> loadScanTree(int scanId) = 0;
    virtual std::unique_ptr<ScanItem> loadLastScanByPath(const QString& path) = 0;

    virtual QList<int> getAvailableScans() const = 0;
    virtual QList<QPair<int, QString>> getAllScanIdsAndPaths() const = 0;

    virtual QString getScanPath(int scanId) const = 0;
    virtual QDateTime getScanTime(int scanId) const = 0;
};

#endif // IDATABASESERVICE_H
