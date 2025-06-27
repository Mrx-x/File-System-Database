#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStandardItemModel>
#include <QDateTime>

#include "Scanner/IScanner.h"
#include "Model/ScanItem.h"
#include "Comporator/TreeComporator.h"

class IDatabaseService;

class ScanController : public QObject
{
    Q_OBJECT

public:
    using DatabasePtr = std::shared_ptr<IDatabaseService>;
    using ScanItemPtr = std::unique_ptr<ScanItem>;

public:
    ScanController(IScanner* scanner, const DatabasePtr& database, QObject* parent = nullptr);

public:
    QStandardItemModel* model() const;

public slots:
    void doScan(const QString& path);
    void doSave();
    void doLoad();
    void doCompare(const QString& path);

signals:
    void compareFinished(const std::vector<ChangeEntry>& changes);

private:
    void addNodeToModel(const ScanItem* item, QStandardItem* parent);
    void buildModel(const ScanItem* root);
    QString convertFileSizeToHumanReadable(const qint64 & bytes) const;
    void clearModel() const;

private:
    IScanner* _scanner;
    QStandardItemModel* _model;
    DatabasePtr _database;
    ScanItemPtr _lastScanItem;
    QString _lastPath;
    QDateTime _lastTime;
};

#endif // SCANCONTROLLER_H
