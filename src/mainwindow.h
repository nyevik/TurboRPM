/**
 * @file mainwindow.h
 * @author Nikolay Yevik
 * @brief Declaration of the MainWindow class for TurboRPM Package Manager Prototype.
 * @version 0.0.1
 * @date 2025-12-6
 */
#pragma once

#include <QMainWindow>
#include <QVector>

class QLineEdit;
class QTableView;
class QSortFilterProxyModel;
class QPushButton;

#include "packagemodel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void refreshPackages();
    void onSearchTextChanged(const QString &text);

    void onDnfCheckUpdate();
    void onInstallPackage();
    void onRemovePackage();
    void onShowPackageFiles();
    void onShowPackageDescription();
    void onWhatProvides();

private:
    QVector<PackageInfo> queryInstalledPackages() const;
    QString runCommand(const QString &program, const QStringList &arguments, int &exitCode) const;
    void showTextDialog(const QString &title, const QString &text) const;
    PackageInfo currentSelectedPackage() const;

    QLineEdit *m_searchEdit = nullptr;
    QTableView *m_tableView = nullptr;
    QPushButton *m_btnRefresh = nullptr;
    QPushButton *m_btnCheckUpdate = nullptr;
    QPushButton *m_btnInstall = nullptr;
    QPushButton *m_btnRemove = nullptr;
    QPushButton *m_btnPkgFiles = nullptr;
    QPushButton *m_btnPkgDesc = nullptr;
    QPushButton *m_btnWhatProvides = nullptr;

    PackageTableModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxy = nullptr;
};
