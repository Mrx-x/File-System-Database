#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Comporator/TreeComporator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class ScanController;
class IDatabaseService;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    using DatabasePtr = std::shared_ptr<IDatabaseService>;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void onCompareFinished(const std::vector<ChangeEntry>& changes);

private:
    void populateChangeList(const std::vector<ChangeEntry>& changes);

private:
    Ui::MainWindow *ui;
    ScanController* _controller;
    DatabasePtr _database;
};
#endif // MAINWINDOW_H
