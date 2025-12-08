/**
 * @file mainwindow.cpp
 * @author Nikolay Yevik
 * @brief Implementation of the MainWindow class for TurboRPM Package Manager Prototype.
 * @version 0.0.1
 * @date 2025-12-6
 */

#include "mainwindow.h"
#include "packagemodel.h"

#include <QHeaderView>
#include <QApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>
#include <QPlainTextEdit>
#include <QProcess>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include<QThreadStorage> //* for thread local storage */
#include <QThread> //* for QThread */
#include <QTextStream>
#include <QScreen>
#include <QScrollBar>
#include <QStyle>
#include <QDateTime>
#include <QTimeZone>
#include <QSet>

#include <iostream>
#include <chrono>

//using Milliseconds = std::chrono::milliseconds;

// File-local constants go here:
namespace {
    constexpr int WaitForStartedTimeoutMs {5000};   // 5 s
    constexpr int WaitForFinishedTimeoutMs {60000};  // 60 s
}


/** Constructor */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("TurboRPM (Prototype)"));
    resize(900, 600);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    /** Top bar: search + refresh */
    auto *topLayout = new QHBoxLayout();
    m_searchEdit = new QLineEdit(central);
    m_searchEdit->setPlaceholderText(QStringLiteral("Search package name..."));
    QFont baseFont = m_searchEdit->font();
    QFont placeholderFont = baseFont;
    placeholderFont.setItalic(true);
    m_searchEdit->setFont(placeholderFont);

    QPalette pal = m_searchEdit->palette();
    pal.setColor(QPalette::PlaceholderText, QColor(128, 128, 128));
    m_searchEdit->setPalette(pal);

    connect(m_searchEdit, &QLineEdit::textChanged, this, [this, baseFont, placeholderFont](const QString &text) {
        m_searchEdit->setFont(text.isEmpty() ? placeholderFont : baseFont);
    });
    m_searchEdit->setClearButtonEnabled(true);



    m_btnRefresh = new QPushButton(QStringLiteral("Refresh installed"), central);

    topLayout->addWidget(m_searchEdit, /*stretch*/ 1);
    topLayout->addWidget(m_btnRefresh);

    mainLayout->addLayout(topLayout);

    /** Table view */
    m_model = new PackageTableModel(this);
    m_proxy = new QSortFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy->setFilterKeyColumn(PackageTableModel::NameColumn);

    m_tableView = new QTableView(central);
    m_tableView->setModel(m_proxy);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    mainLayout->addWidget(m_tableView);

    /** Bottom buttons */
    auto *bottomLayout = new QHBoxLayout();

    m_btnCheckUpdate = new QPushButton(QStringLiteral("DNF check-update"), central);
    m_btnInstall = new QPushButton(QStringLiteral("Install pkg..."), central);
    m_btnRemove = new QPushButton(QStringLiteral("Remove pkg..."), central);
    m_btnPkgFiles = new QPushButton(QStringLiteral("Files in selected pkg"), central);
    m_btnPkgDesc = new QPushButton(QStringLiteral("Description of selected pkg"), central);
    m_btnWhatProvides = new QPushButton(QStringLiteral("What provides file..."), central);

    bottomLayout->addWidget(m_btnCheckUpdate);
    bottomLayout->addWidget(m_btnInstall);
    bottomLayout->addWidget(m_btnRemove);
    bottomLayout->addWidget(m_btnPkgFiles);
    bottomLayout->addWidget(m_btnPkgDesc);
    bottomLayout->addWidget(m_btnWhatProvides);
    bottomLayout->addStretch();

    mainLayout->addLayout(bottomLayout);

    setCentralWidget(central);

    /** Connections -> Slots to signals */
    connect(m_btnRefresh, &QPushButton::clicked, this, &MainWindow::refreshPackages);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    connect(m_btnCheckUpdate, &QPushButton::clicked, this, &MainWindow::onDnfCheckUpdate);
    connect(m_btnInstall, &QPushButton::clicked, this, &MainWindow::onInstallPackage);
    connect(m_btnRemove, &QPushButton::clicked, this, &MainWindow::onRemovePackage);
    connect(m_btnPkgFiles, &QPushButton::clicked, this, &MainWindow::onShowPackageFiles);
    connect(m_btnPkgDesc, &QPushButton::clicked, this, &MainWindow::onShowPackageDescription);
    connect(m_btnWhatProvides, &QPushButton::clicked, this, &MainWindow::onWhatProvides);

    // Initial load
    refreshPackages();
}

void MainWindow::refreshPackages()
{
    QVector<PackageInfo> pkgs = queryInstalledPackages();
    m_model->setPackages(pkgs);
    m_tableView->resizeColumnsToContents();

    const auto *screen = QGuiApplication::primaryScreen();
    const QRect available = screen ? screen->availableGeometry() : QRect();

    const int scrollbarWidth = m_tableView->verticalScrollBar()->isVisible()
                                   ? m_tableView->verticalScrollBar()->sizeHint().width()
                                   : m_tableView->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    const int preferredWidth = m_tableView->horizontalHeader()->length()
                               + m_tableView->verticalHeader()->width()
                               + scrollbarWidth
                               + (m_tableView->frameWidth() * 2)
                               + 40; // extra padding for layout margins

    const int maxWidth = available.isValid() ? available.width() - 20 : preferredWidth;
    const int finalWidth = qMin(qMax(preferredWidth, 700), maxWidth);

    const int preferredHeight = qMax(height(), 600);
    const int maxHeight = available.isValid() ? available.height() - 20 : preferredHeight;
    const int finalHeight = qMin(preferredHeight, maxHeight);

    resize(finalWidth, finalHeight);
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    m_proxy->setFilterFixedString(text);
}

QVector<PackageInfo> MainWindow::queryInstalledPackages() const
{
#ifdef QT_DEBUG
    QDebug dbg(QtDebugMsg);//on stack
    
    dbg.setVerbosity(QDebug::MaximumVerbosity);
    dbg << "Entering MainWindow::queryInstalledPackages()";

    //std::unique_ptr<QDebug> uni_qdebugobj_ptr = std::make_unique<QDebug>(QtDebugMsg);
    //QDebug *raw_qdebugobj_ptr = uni_qdebugobj_ptr.get(); //raw pointer for easier use below
    //std::cout << "QDebug verbosity from  uni_qdebugobj_ptr" << uni_qdebugobj_ptr->verbosity() << std::endl;
    //std::cout << "QDebug verbosity from raw_qdebugobj_ptr" << raw_qdebugobj_ptr->verbosity( << std::endl;
    
    dbg.setVerbosity(QDebug::MaximumVerbosity);
    dbg << "QDebug verbosity from dbg object " << dbg.verbosity() << "\n";
    
#endif

    //QThread *currentThread = QThread::currentThread();
    QVector<PackageInfo> result;

    static constexpr QChar kFieldSep(u'\x1F'); // unit separator to avoid clashing with tabs/spaces in fields

    QProcess proc; /** to run the command I need */
    proc.setProcessChannelMode(QProcess::SeparateChannels); //* we want to read stdout and stderr separately */
    
    QStringList args;
    //const bool hasRepoTag = rpmSupportsFromRepoTag();
    //const QString repoField = hasRepoTag ? QString::fromLatin1("\x1F%{repoid}")
   //                                      : QString::fromLatin1("\x1F");
    /*args << "-qa"
         << "--qf"
         << QString::fromLatin1("%{NAME}\x1F%{VERSION}-%{RELEASE}\x1F%{ARCH}\x1F%{INSTALLTIME:date}\x1F%{GROUP}\x1F%{SIZE}")
                //+ repoField
                + QString::fromLatin1("\x1F%{SUMMARY}\n");*/

    QString command = QStringLiteral("dnf");

    //Better to use DNF to get more accurate info about installed packages
    const QString queryFormat = QStringLiteral(
        "%{name}\x1F" // rpm package name
        "%{version}-%{release}\x1F"
        "%{arch}\x1F"
        "%{installtime}\x1F"
        "%{group}\x1F"
        "%{size}\x1F" //size in bytes
        "%{from_repo}\x1F" //attempt to resolve what repo this package is coming from
        "%{summary}");

    args << QStringLiteral("repoquery")
         << QStringLiteral("--installed")
         << QStringLiteral("--qf") << queryFormat;


    /**command might not start so we set a timeout */
    proc.start(command, args); /** START THE PROCESS */

    if (!proc.waitForStarted(WaitForStartedTimeoutMs)) {
        QMessageBox::warning(nullptr, tr("Error"),
                             tr("Failed to start dnf process."));
        return result;
    }
    /** command might be slow or hang, so we set a timeout */
    if (!proc.waitForFinished(WaitForFinishedTimeoutMs)) { // 60 s timeout
        QMessageBox::warning(nullptr, tr("Error"),
                             tr("Timed out while running dnf"));
        return result;
    }
    if (proc.exitStatus() != QProcess::NormalExit) {
        QMessageBox::warning(nullptr, tr("Error"),
                             tr("dnf crashed while running."));
        return result;
    }

    const QByteArray out = proc.readAllStandardOutput(); //* read stdout  into QByteArray */
    const QByteArray err = proc.readAllStandardError();  //* read stderr into QByteArray */
   
    #ifdef QT_DEBUG
    qDebug() << "dnf repoquery output bytes:" << out.size() << "stderr bytes:" << err.size();
    if (!err.isEmpty())
        qDebug() << "dnf repoquery stderr:" << err;
    #endif

    QString data = QString::fromLocal8Bit(out);
    QTextStream stream(&data, QIODevice::ReadOnly);

    QString line;
    int lineIndex = 0;
    // for deduplicating (name, version, arch)
    QSet<QString> seenKeys;

    while (stream.readLineInto(&line)) {
        const QString trimmed = line.trimmed();
        if (trimmed.isEmpty()) {
#ifdef QT_DEBUG
            dbg << "Skipping empty line at index "<< lineIndex << " because it is empty \n";
#endif
            ++lineIndex;
            continue;
        }
        // Filter known dnf informational noise printed to stdout
        if (trimmed.startsWith(
                QStringLiteral("Not root, Subscription Management repositories not updated"))) {
#ifdef QT_DEBUG
            dbg << "Skipping dnf info line at index because it is informational: " << lineIndex << ":" << trimmed << "\n";
#endif
            ++lineIndex;
            continue;
        }
        const QStringList fields = trimmed.split(kFieldSep, Qt::KeepEmptyParts);
        // Allow partially filled records, but require at least:
        //   0: name, 1: version-release, 2: arch
        if (fields.size() < 3) {
#ifdef QT_DEBUG
            dbg << "Skipping line with too few fields at index" << lineIndex << ":" << trimmed << "\n";
#endif
            ++lineIndex;
            continue;
        }

        PackageInfo pkg;
        pkg.name    = fields.value(0).trimmed();
        pkg.version = fields.value(1).trimmed();
        pkg.arch    = fields.value(2).trimmed();
        // Minimal validation: if these are missing it's not a real package entry
        if (pkg.name.isEmpty() || pkg.version.isEmpty() || pkg.arch.isEmpty()) {
#ifdef QT_DEBUG
            dbg << "Skipping line with empty mandatory fields at index"
                     << lineIndex << ":" << trimmed <<"\n";
#endif
            ++lineIndex;
            continue;
        }
        // Deduplicate (dnf can sometimes output multiple rows for same NEVRA)
        const QString key =
            pkg.name + QLatin1Char('|') + pkg.version + QLatin1Char('|') + pkg.arch;
        if (seenKeys.contains(key)) {
#ifdef QT_DEBUG
            dbg << "Skipping duplicate entry for" << key << "at line index" << lineIndex << "\n";
#endif
            ++lineIndex;
            continue;
        }
        seenKeys.insert(key);

        // INSTALLTIME comes from dnf repoquery as a preformatted string
        // (see dnf-plugins-core repoquery.py: PackageWrapper.installtime).
        // We keep it as-is instead of trying to parse epoch seconds.
        const QString installField = fields.value(3).trimmed();
        pkg.installDate = installField;

        //Do not like how install date is represented, convert UTC string to local time
        /*const QDateTime dt = QDateTime::fromString(installField, Qt::ISODate);
        if (dt.isValid()) {
            pkg.installDate = dt.toLocalTime().toString(QStringLiteral("yyyy-MM-dd HH:mm"));
        }*/

        pkg.group   = fields.value(4).trimmed();
        
        // SIZE: keep original string, but sanity-check that it's numeric
        const QString sizeField = fields.value(5).trimmed();
        bool okSize = false;
        (void)sizeField.toLongLong(&okSize);
        if (okSize) {
            pkg.size = sizeField;
        } else {
            pkg.size.clear(); // treat nonsense size as "unknown"
        }

        #ifdef QT_DEBUG
        //dbg << "Parsed dnf line" << lineIndex << "fields" << fields;
        #endif

        pkg.repo    = fields.value(6).trimmed();
        pkg.summary = fields.value(7).trimmed();

        result.push_back(pkg);
        ++lineIndex;
    }//end of while reading lines

    return result;
}//=== End of MainWindow::queryInstalledPackages ===



QString MainWindow::runCommand(const QString &program,
                               const QStringList &arguments,
                               int &exitCode) const
{
    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(program, arguments);

    if (!proc.waitForFinished(-1)) {
        exitCode = -1;
        return QStringLiteral("Command timed out or failed to start.");
    }

    exitCode = proc.exitCode();
    return QString::fromLocal8Bit(proc.readAll());
}

void MainWindow::showTextDialog(const QString &title, const QString &text) const
{
    QDialog dlg(const_cast<MainWindow*>(this));
    dlg.setWindowTitle(title);
    dlg.resize(800, 600);

    auto *layout = new QVBoxLayout(&dlg);
    auto *edit = new QPlainTextEdit(&dlg);
    edit->setReadOnly(true);
    edit->setPlainText(text);
    layout->addWidget(edit);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    layout->addWidget(buttons);

    dlg.exec();
}

PackageInfo MainWindow::currentSelectedPackage() const
{
    QModelIndex proxyIndex = m_tableView->currentIndex();
    if (!proxyIndex.isValid())
        return {};

    QModelIndex sourceIndex = m_proxy->mapToSource(proxyIndex);
    if (!sourceIndex.isValid())
        return {};

    return m_model->packageAt(sourceIndex.row());
}

// === Slots for actions ===

void MainWindow::onDnfCheckUpdate()
{
    int exitCode = 0;
    QString output = runCommand("dnf", {"check-update"}, exitCode);

    QString title = tr("dnf check-update (exit %1)").arg(exitCode);
    showTextDialog(title, output);
}

void MainWindow::onInstallPackage()
{
    bool ok = false;
    QString pkgName = QInputDialog::getText(this, tr("Install package"),
                                            tr("Package name to install:"),
                                            QLineEdit::Normal,
                                            QString(), &ok);
    if (!ok || pkgName.trimmed().isEmpty())
        return;

    int exitCode = 0;
    QString output = runCommand("dnf", {"install", "-y", pkgName.trimmed()}, exitCode);

    QString title = tr("dnf install %1 (exit %2)")
                        .arg(pkgName.trimmed())
                        .arg(exitCode);
    showTextDialog(title, output);
    if (exitCode == 0)
        refreshPackages();
}

void MainWindow::onRemovePackage()
{
    bool ok = false;
    QString pkgName = QInputDialog::getText(this, tr("Remove package"),
                                            tr("Package name to remove:"),
                                            QLineEdit::Normal,
                                            QString(), &ok);
    if (!ok || pkgName.trimmed().isEmpty())
        return;

    int exitCode = 0;
    QString output = runCommand("dnf", {"remove", "-y", pkgName.trimmed()}, exitCode);

    QString title = tr("dnf remove %1 (exit %2)")
                        .arg(pkgName.trimmed())
                        .arg(exitCode);
    showTextDialog(title, output);
    if (exitCode == 0)
        refreshPackages();
}

void MainWindow::onShowPackageFiles()
{
    PackageInfo pkg = currentSelectedPackage();
    if (pkg.name.isEmpty()) {
        QMessageBox::information(this, tr("No selection"),
                                 tr("Select a package first."));
        return;
    }

    int exitCode = 0;
    QString output = runCommand("rpm", {"-ql", pkg.name}, exitCode);

    QString title = tr("Files in %1 (exit %2)")
                        .arg(pkg.name)
                        .arg(exitCode);
    showTextDialog(title, output);
}

void MainWindow::onShowPackageDescription()
{
    PackageInfo pkg = currentSelectedPackage();
    if (pkg.name.isEmpty()) {
        QMessageBox::information(this, tr("No selection"),
                                 tr("Select a package first."));
        return;
    }

    int exitCode = 0;
    QString output = runCommand("rpm", {"-qi", pkg.name}, exitCode);

    QString title = tr("Description for %1 (exit %2)")
                        .arg(pkg.name)
                        .arg(exitCode);
    showTextDialog(title, output);
}

void MainWindow::onWhatProvides()
{
    QString path = QFileDialog::getOpenFileName(this,
                                               tr("Select file to query"),
                                               QString());

    if (path.trimmed().isEmpty()) {
        bool ok = false;
        path = QInputDialog::getText(this, tr("What provides this file"),
                                     tr("Full path to file:"),
                                     QLineEdit::Normal,
                                     QString(), &ok);
        if (!ok || path.trimmed().isEmpty())
            return;
    }

    const QString trimmedPath = path.trimmed();
    int exitCode = 0;
    QString output = runCommand("rpm", {"-qf", trimmedPath}, exitCode);

    QString title = tr("rpm -qf %1 (exit %2)")
                        .arg(trimmedPath)
                        .arg(exitCode);
    showTextDialog(title, output);
}
