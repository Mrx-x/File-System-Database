#ifndef SCANCONTROLLER_H
#define SCANCONTROLLER_H

#include <QObject>
#include <QString>
#include <QStandardItemModel>

#include <Scanner/IScanner.h>

class ScanController : public QObject
{
    Q_OBJECT

public:
    ScanController(IScanner* scanner, QObject* parent = nullptr);

public:
    QStandardItemModel* model() const;

public slots:
    void startScan(const QString& path);

private:
    void addNodeToModel(const ScanItem* item, QStandardItem* parent);
    void buildModel(const ScanItem* root);
    QString convertFileSizeToHumanReadable(const qint64 & bytes) const;

private:
    IScanner* _scanner;
    QStandardItemModel* _model;
};

#endif // SCANCONTROLLER_H
