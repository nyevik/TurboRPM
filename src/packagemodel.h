/**
 * @author Nikolay Yevik
 * @date 2025
 * @brief Model for representing RPM package information in a table view
 * This file is part of a Qt-based application for managing RPM packages.
 * It defines a data MODEL that holds package information such as name,
 * version, and architecture, and provides the necessary interface for displaying
 * this information in a Qt VIEW component.
 */


#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

struct PackageInfo {
    QString name; /** RPM package name */
    QString version;  /** VERSION-RELEASE */
    QString arch; /** Architecture such as x86_64, noarch, etc. */
    QString installDate; /** Human-readable install date */
    QString group; /** DNF/RPM group (e.g., Development) */
    QString size; /** Size on disk (as reported by rpm) */
    qint64 sizeBytes = -1; /** Raw size in bytes for conversions */
    QString repo; /** Repository name rpm says it came from */
    QString summary; /** Package summary description */
};

class PackageTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit PackageTableModel(QObject *parent = nullptr);

    enum Column {
        NameColumn = 0,
        VersionColumn,
        ArchColumn,
        InstallDateColumn,
        GroupColumn,
        SizeColumn,
        RepoColumn,
        SummaryColumn,
        ColumnCount
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    void setPackages(const QVector<PackageInfo> &pkgs);
    PackageInfo packageAt(int row) const;
    void updateSizeDisplay(int row, const QString &displayValue);

private:
    QVector<PackageInfo> m_pkgs;
};
