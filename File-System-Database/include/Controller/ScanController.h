#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStandardItemModel>

#include <Scanner/IScanner.h>

class IDatabaseService;

class ScanController : public QObject
{
    Q_OBJECT

public:
    using DatabasePtr = std::shared_ptr<IDatabaseService>;

public:
    ScanController(IScanner* scanner, const DatabasePtr& database, QObject* parent = nullptr);

public:
    QStandardItemModel* model() const;

public slots:
    void startScan(const QString& path);
    void doLoad(int scanId);

private:
    void addNodeToModel(const ScanItem* item, QStandardItem* parent);
    void buildModel(const ScanItem* root);
    QString convertFileSizeToHumanReadable(const qint64 & bytes) const;

private:
    IScanner* _scanner;
    QStandardItemModel* _model;
    DatabasePtr _database;
};

#endif // SCANCONTROLLER_H
