#ifndef ISCANNER_H
#define ISCANNER_H

#include <QObject>
#include <QPushButton>
#include <QString>

class ScanItem;

class IScanner : public QObject
{
    Q_OBJECT
public:
    explicit IScanner(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~IScanner() = default;
public:
    virtual std::unique_ptr<ScanItem> scan(const QString& path) const = 0;
};

#endif // ISCANNER_H
