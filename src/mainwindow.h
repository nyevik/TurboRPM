/**
 * @file mainwindow.h
 * @author Nikolay Yevik
 * @brief Declaration of the MainWindow class for TurboRPM Package Manager Prototype.
 * @version 0.0.1
 * @date 2025-12-6
 */
#pragma once

#include <QMainWindow>
#include <QModelIndex>
#include <QPoint>
#include <QVector>
#include <QPair>

class QLineEdit;
class QTableView;
class QSortFilterProxyModel;
class QPushButton;
class QFrame;
class QLabel;
class QEvent;

#include "packagemodel.h"

enum class SizeUnit {
    Kilobytes,
    Megabytes
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void refreshPackages();
    void onSearchTextChanged(const QString &text);

    void onDnfCheckUpdate();
    void onInstallPackage();
    void onRemovePackage();
    void onShowPackageFiles();
    void onShowPackageDescription();
    void onWhatProvides();
    void onWhatProvidesDnD();
    void onTableContextMenu(const QPoint &pos);

    // Context menu actions
    void onNameGetMoreInfo();
    void onNameCheckUpdates();
    void onConvertSizeToKB();
    void onConvertSizeToMB();

private:
    QVector<PackageInfo> queryInstalledPackages() const;
    QString runCommand(const QString &program, const QStringList &arguments, int &exitCode) const;
    void showTextDialog(const QString &title, const QString &text) const;
    void showPackageInfoTable(const QString &pkgName,
                              const QVector<QPair<QString, QString>> &fields) const;
    PackageInfo currentSelectedPackage() const;
    void startColumnConversion(SizeUnit unit);

    QLineEdit *m_searchEdit = nullptr;
    QTableView *m_tableView = nullptr;
    QPushButton *m_btnRefresh = nullptr;
    QPushButton *m_btnCheckUpdate = nullptr;
    QPushButton *m_btnInstall = nullptr;
    QPushButton *m_btnRemove = nullptr;
    QPushButton *m_btnPkgFiles = nullptr;
    QPushButton *m_btnPkgDesc = nullptr;
    QPushButton *m_btnWhatProvides = nullptr;
    QPushButton *m_btnWhatProvidesDnD  = nullptr;
    QFrame *m_dropArea = nullptr;
    QLabel *m_dropLabel = nullptr;

    PackageTableModel *m_model = nullptr;
    QSortFilterProxyModel *m_proxy = nullptr;

    QModelIndex m_lastContextSourceIndex;

    PackageInfo packageFromSourceIndex(const QModelIndex &sourceIndex) const;
    void handleWhatProvidesPaths(const QStringList &paths, const QString &sourceLabel);
};
