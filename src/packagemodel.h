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
    QString summary; /** Package summary desription */
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

private:
    QVector<PackageInfo> m_pkgs;
};
