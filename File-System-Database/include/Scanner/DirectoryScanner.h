#ifndef DIRECTORYSCANNER_H
#define DIRECTORYSCANNER_H

#include "IScanner.h"

class DirectoryScanner : public IScanner
{
    Q_OBJECT
public:
    explicit DirectoryScanner(QObject* parent = nullptr);

public:
    std::unique_ptr<ScanItem> scan(const QString& path) const override;

private:
    std::unique_ptr<ScanItem> scanFolder(const QString& path, ScanItem* parent) const;
};

#endif // DIRECTORYSCANNER_H
