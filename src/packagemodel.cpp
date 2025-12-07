/**
 * @author Nikolay Yevik
 * @brief Implementation of the PackageTableModel class for TurboRPM Package Manager Prototype.
 * @version 0.0.1
 * @date 2025-12-6
 */
#include "packagemodel.h"

PackageTableModel::PackageTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int PackageTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_pkgs.size();
}

int PackageTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ColumnCount;
}

QVariant PackageTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    int row = index.row();
    int col = index.column();
    if (row < 0 || row >= m_pkgs.size())
        return {};

    const PackageInfo &pkg = m_pkgs[row];

    if (role == Qt::DisplayRole) {
        switch (col) {
        case NameColumn:
            return pkg.name;
        case VersionColumn:
            return pkg.version;
        case ArchColumn:
            return pkg.arch;
        case SummaryColumn:
            return pkg.summary;
        default:
            break;
        }
    }

    return {};
}

QVariant PackageTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case NameColumn:
            return QStringLiteral("Name");
        case VersionColumn:
            return QStringLiteral("Version");
        case ArchColumn:
            return QStringLiteral("Arch");
        case SummaryColumn:
            return QStringLiteral("Summary");
        default:
            break;
        }
    }

    return {};
}

void PackageTableModel::setPackages(const QVector<PackageInfo> &pkgs)
{
    beginResetModel();
    m_pkgs = pkgs;
    endResetModel();
}

PackageInfo PackageTableModel::packageAt(int row) const
{
    if (row < 0 || row >= m_pkgs.size())
        return {};
    return m_pkgs[row];
}
