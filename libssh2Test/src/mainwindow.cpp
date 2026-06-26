#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QStyle>
#include <QDir>
#include <QFileInfo>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static QString formatFileSize(qint64 bytes)
{
    if (bytes < 1024)
        return QStringLiteral("%1 B").arg(bytes);
    if (bytes < 1024 * 1024)
        return QStringLiteral("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
    if (bytes < 1024LL * 1024 * 1024)
        return QStringLiteral("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
    return QStringLiteral("%1 GB").arg(bytes / (1024.0 * 1024.0 * 1024.0), 0, 'f', 2);
}

static QPixmap iconForFile(const RemoteFileInfo &info)
{
    return info.isDirectory
        ? qApp->style()->standardIcon(QStyle::SP_DirIcon).pixmap(16, 16)
        : qApp->style()->standardIcon(QStyle::SP_FileIcon).pixmap(16, 16);
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("SSH Remote File Manager"));

    setupUi();
    setConnectedState(false);

    // Worker thread
    workerThread_ = new QThread(this);
    worker_ = new SshWorker;           // no parent — will be moved
    worker_->moveToThread(workerThread_);

    // --- Worker -> UI signals (queued auto because different threads) ---
    connect(worker_, &SshWorker::connected,           this, &MainWindow::onConnected);
    connect(worker_, &SshWorker::disconnected,        this, &MainWindow::onDisconnected);
    connect(worker_, &SshWorker::connectionError,     this, &MainWindow::onConnectionError);
    connect(worker_, &SshWorker::directoryListed,     this, &MainWindow::onDirectoryListed);
    connect(worker_, &SshWorker::downloadProgress,    this, &MainWindow::onDownloadProgress);
    connect(worker_, &SshWorker::downloadFinished,    this, &MainWindow::onDownloadFinished);
    connect(worker_, &SshWorker::uploadProgress,      this, &MainWindow::onUploadProgress);
    connect(worker_, &SshWorker::uploadFinished,      this, &MainWindow::onUploadFinished);
    connect(worker_, &SshWorker::deleteFinished,      this, &MainWindow::onDeleteFinished);
    connect(worker_, &SshWorker::operationError,      this, &MainWindow::onOperationError);

    // Cleanup on thread end
    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);

    workerThread_->start();
}

MainWindow::~MainWindow()
{
    // Cancel any in-flight operation (atomic, safe to call from any thread)
    worker_->cancelOperation();
    QMetaObject::invokeMethod(worker_, "disconnectFromHost", Qt::QueuedConnection);
    workerThread_->quit();
    if (!workerThread_->wait(5000))
        workerThread_->terminate();
}

// ---------------------------------------------------------------------------
// UI setup
// ---------------------------------------------------------------------------

void MainWindow::setupUi()
{
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *rootLayout = new QVBoxLayout(central);
    rootLayout->setSpacing(6);

    // ---- Connection row ----
    auto *connGroup = new QGroupBox(QStringLiteral("Connection"));
    auto *connLayout = new QHBoxLayout(connGroup);

    hostEdit_ = new QLineEdit;
    hostEdit_->setPlaceholderText(QStringLiteral("Host"));
    portEdit_ = new QLineEdit(QStringLiteral("22"));
    portEdit_->setMaximumWidth(60);
    portEdit_->setPlaceholderText(QStringLiteral("Port"));
    userEdit_ = new QLineEdit;
    userEdit_->setPlaceholderText(QStringLiteral("User"));
    passwordEdit_ = new QLineEdit;
    passwordEdit_->setPlaceholderText(QStringLiteral("Password"));
    passwordEdit_->setEchoMode(QLineEdit::Password);

    connectBtn_ = new QPushButton(QStringLiteral("Connect"));
    disconnectBtn_ = new QPushButton(QStringLiteral("Disconnect"));

    connect(connectBtn_,    &QPushButton::clicked, this, &MainWindow::onConnect);
    connect(disconnectBtn_, &QPushButton::clicked, this, &MainWindow::onDisconnect);
    connect(passwordEdit_,  &QLineEdit::returnPressed, this, &MainWindow::onConnect);

    connLayout->addWidget(hostEdit_);
    connLayout->addWidget(portEdit_);
    connLayout->addWidget(userEdit_);
    connLayout->addWidget(passwordEdit_);
    connLayout->addWidget(connectBtn_);
    connLayout->addWidget(disconnectBtn_);
    rootLayout->addWidget(connGroup);

    // ---- Path navigation ----
    auto *pathLayout = new QHBoxLayout;
    pathEdit_ = new QLineEdit(QStringLiteral("/"));
    homeBtn_  = new QPushButton(QStringLiteral("Home"));
    upBtn_    = new QPushButton(QStringLiteral("Up"));
    refreshBtn_ = new QPushButton(QStringLiteral("Refresh"));

    connect(homeBtn_,    &QPushButton::clicked, this, &MainWindow::onGoHome);
    connect(upBtn_,      &QPushButton::clicked, this, &MainWindow::onGoUp);
    connect(refreshBtn_, &QPushButton::clicked, this, &MainWindow::onRefresh);
    connect(pathEdit_,   &QLineEdit::returnPressed, this, [this]() {
        navigateTo(pathEdit_->text());
    });

    pathLayout->addWidget(new QLabel(QStringLiteral("Path:")));
    pathLayout->addWidget(pathEdit_);
    pathLayout->addWidget(homeBtn_);
    pathLayout->addWidget(upBtn_);
    pathLayout->addWidget(refreshBtn_);
    rootLayout->addLayout(pathLayout);

    // ---- File table ----
    fileTable_ = new QTableWidget(0, 4);
    fileTable_->setHorizontalHeaderLabels({
        QStringLiteral("Name"), QStringLiteral("Size"),
        QStringLiteral("Modified"), QStringLiteral("Permissions")
    });
    fileTable_->horizontalHeader()->setStretchLastSection(true);
    fileTable_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    fileTable_->verticalHeader()->setVisible(false);
    fileTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    fileTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    fileTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    fileTable_->setSortingEnabled(true);
    fileTable_->sortByColumn(0, Qt::AscendingOrder);
    fileTable_->setIconSize(QSize(16, 16));

    connect(fileTable_, &QTableWidget::cellDoubleClicked,
            this, &MainWindow::onTableDoubleClicked);
    connect(fileTable_, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onSelectionChanged);

    rootLayout->addWidget(fileTable_, /*stretch=*/1);

    // ---- Operations row ----
    auto *opLayout = new QHBoxLayout;
    downloadBtn_ = new QPushButton(QStringLiteral("Download"));
    uploadBtn_   = new QPushButton(QStringLiteral("Upload"));
    deleteBtn_   = new QPushButton(QStringLiteral("Delete"));
    renameBtn_   = new QPushButton(QStringLiteral("Rename"));
    mkdirBtn_    = new QPushButton(QStringLiteral("New Folder"));
    cancelBtn_   = new QPushButton(QStringLiteral("Cancel"));

    connect(downloadBtn_, &QPushButton::clicked, this, &MainWindow::onDownload);
    connect(uploadBtn_,   &QPushButton::clicked, this, &MainWindow::onUpload);
    connect(deleteBtn_,   &QPushButton::clicked, this, &MainWindow::onDelete);
    connect(renameBtn_,   &QPushButton::clicked, this, &MainWindow::onRename);
    connect(mkdirBtn_,    &QPushButton::clicked, this, &MainWindow::onMkdir);
    connect(cancelBtn_,   &QPushButton::clicked, this, &MainWindow::onCancel);

    opLayout->addWidget(downloadBtn_);
    opLayout->addWidget(uploadBtn_);
    opLayout->addWidget(deleteBtn_);
    opLayout->addWidget(renameBtn_);
    opLayout->addWidget(mkdirBtn_);
    opLayout->addStretch();
    opLayout->addWidget(cancelBtn_);
    rootLayout->addLayout(opLayout);

    // ---- Progress bar ----
    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    progressBar_->setVisible(false);
    rootLayout->addWidget(progressBar_);

    // ---- Status bar ----
    statusBar()->showMessage(QStringLiteral("Disconnected"));
}

// ---------------------------------------------------------------------------
// State helpers
// ---------------------------------------------------------------------------

void MainWindow::setConnectedState(bool connected)
{
    hostEdit_->setEnabled(!connected);
    portEdit_->setEnabled(!connected);
    userEdit_->setEnabled(!connected);
    passwordEdit_->setEnabled(!connected);
    connectBtn_->setEnabled(!connected);
    disconnectBtn_->setEnabled(connected);

    pathEdit_->setEnabled(connected);
    homeBtn_->setEnabled(connected);
    upBtn_->setEnabled(connected);
    refreshBtn_->setEnabled(connected);
    fileTable_->setEnabled(connected);

    uploadBtn_->setEnabled(connected);
    mkdirBtn_->setEnabled(connected);
    cancelBtn_->setEnabled(false);

    if (!connected) {
        fileTable_->setRowCount(0);
        pathEdit_->setText(QStringLiteral("/"));
        progressBar_->setVisible(false);
    }

    onSelectionChanged();  // update row-dependent buttons
}

void MainWindow::setBusy(bool busy)
{
    cancelBtn_->setEnabled(busy);
    downloadBtn_->setEnabled(!busy);
    uploadBtn_->setEnabled(!busy);
    deleteBtn_->setEnabled(!busy);
    renameBtn_->setEnabled(!busy);
    mkdirBtn_->setEnabled(!busy);
    refreshBtn_->setEnabled(!busy);
    progressBar_->setVisible(busy);
}

QString MainWindow::currentRemotePath() const
{
    return pathEdit_->text();
}

void MainWindow::navigateTo(const QString &path)
{
    pathEdit_->setText(path);
    statusBar()->showMessage(QStringLiteral("Loading %1 ...").arg(path));
    QMetaObject::invokeMethod(worker_, "listDirectory", Qt::QueuedConnection,
                              Q_ARG(QString, path));
}

void MainWindow::populateTable(const QList<RemoteFileInfo> &files)
{
    fileTable_->setSortingEnabled(false);
    fileTable_->setRowCount(0);

    for (const auto &f : files) {
        int row = fileTable_->rowCount();
        fileTable_->insertRow(row);

        auto *nameItem = new QTableWidgetItem(iconForFile(f), f.name);
        nameItem->setData(Qt::UserRole, f.isDirectory);
        fileTable_->setItem(row, 0, nameItem);

        QString sizeStr = f.isDirectory ? QString() : formatFileSize(f.size);
        fileTable_->setItem(row, 1, new QTableWidgetItem(sizeStr));

        QString timeStr = f.modificationTime.isValid()
            ? f.modificationTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"))
            : QString();
        fileTable_->setItem(row, 2, new QTableWidgetItem(timeStr));

        fileTable_->setItem(row, 3, new QTableWidgetItem(f.permissions));
    }

    fileTable_->setSortingEnabled(true);
}

// ---------------------------------------------------------------------------
// UI slots — connection
// ---------------------------------------------------------------------------

void MainWindow::onConnect()
{
    QString host = hostEdit_->text().trimmed();
    if (host.isEmpty()) {
        statusBar()->showMessage(QStringLiteral("Enter a hostname"), 3000);
        return;
    }

    bool ok;
    quint16 port = static_cast<quint16>(portEdit_->text().toUShort(&ok));
    if (!ok) port = 22;

    statusBar()->showMessage(QStringLiteral("Connecting to %1:%2 ...").arg(host).arg(port));
    connectBtn_->setEnabled(false);

    QMetaObject::invokeMethod(worker_, "connectToHost", Qt::QueuedConnection,
                              Q_ARG(QString, host),
                              Q_ARG(quint16, port),
                              Q_ARG(QString, userEdit_->text()),
                              Q_ARG(QString, passwordEdit_->text()));
}

void MainWindow::onDisconnect()
{
    QMetaObject::invokeMethod(worker_, "disconnectFromHost", Qt::QueuedConnection);
}

// ---------------------------------------------------------------------------
// UI slots — navigation
// ---------------------------------------------------------------------------

void MainWindow::onRefresh()
{
    navigateTo(currentRemotePath());
}

void MainWindow::onGoUp()
{
    QString path = currentRemotePath();
    if (path == QLatin1String("/"))
        return;
    int idx = path.lastIndexOf(QLatin1Char('/'));
    navigateTo(idx == 0 ? QStringLiteral("/") : path.left(idx));
}

void MainWindow::onGoHome()
{
    navigateTo(QStringLiteral("/"));
}

// ---------------------------------------------------------------------------
// UI slots — operations
// ---------------------------------------------------------------------------

void MainWindow::onDownload()
{
    int row = fileTable_->currentRow();
    if (row < 0) return;

    QString name = fileTable_->item(row, 0)->text();
    bool isDir = fileTable_->item(row, 0)->data(Qt::UserRole).toBool();
    if (isDir) {
        statusBar()->showMessage(QStringLiteral("Cannot download a directory"), 3000);
        return;
    }

    QString remotePath = currentRemotePath();
    if (!remotePath.endsWith(QLatin1Char('/')))
        remotePath += QLatin1Char('/');
    remotePath += name;

    QString localPath = QFileDialog::getSaveFileName(this,
        QStringLiteral("Save as"), QDir::homePath() + QLatin1Char('/') + name);
    if (localPath.isEmpty())
        return;

    setBusy(true);
    statusBar()->showMessage(QStringLiteral("Downloading %1 ...").arg(name));
    QMetaObject::invokeMethod(worker_, "downloadFile", Qt::QueuedConnection,
                              Q_ARG(QString, remotePath),
                              Q_ARG(QString, localPath));
}

void MainWindow::onUpload()
{
    QString localPath = QFileDialog::getOpenFileName(this,
        QStringLiteral("Select file to upload"));
    if (localPath.isEmpty())
        return;

    QString remotePath = currentRemotePath();
    if (!remotePath.endsWith(QLatin1Char('/')))
        remotePath += QLatin1Char('/');
    remotePath += QFileInfo(localPath).fileName();

    setBusy(true);
    statusBar()->showMessage(QStringLiteral("Uploading %1 ...").arg(QFileInfo(localPath).fileName()));
    QMetaObject::invokeMethod(worker_, "uploadFile", Qt::QueuedConnection,
                              Q_ARG(QString, localPath),
                              Q_ARG(QString, remotePath));
}

void MainWindow::onDelete()
{
    int row = fileTable_->currentRow();
    if (row < 0) return;

    QString name = fileTable_->item(row, 0)->text();
    bool isDir = fileTable_->item(row, 0)->data(Qt::UserRole).toBool();

    QString remotePath = currentRemotePath();
    if (!remotePath.endsWith(QLatin1Char('/')))
        remotePath += QLatin1Char('/');
    remotePath += name;

    auto btn = QMessageBox::question(this,
        QStringLiteral("Confirm"),
        QStringLiteral("Delete %1?").arg(remotePath));
    if (btn != QMessageBox::Yes)
        return;

    statusBar()->showMessage(QStringLiteral("Deleting %1 ...").arg(name));
    if (isDir) {
        QMetaObject::invokeMethod(worker_, "deleteDirectory", Qt::QueuedConnection,
                                  Q_ARG(QString, remotePath));
    } else {
        QMetaObject::invokeMethod(worker_, "deleteFile", Qt::QueuedConnection,
                                  Q_ARG(QString, remotePath));
    }
}

void MainWindow::onRename()
{
    int row = fileTable_->currentRow();
    if (row < 0) return;

    QString oldName = fileTable_->item(row, 0)->text();
    bool ok;
    QString newName = QInputDialog::getText(this,
        QStringLiteral("Rename"), QStringLiteral("New name:"),
        QLineEdit::Normal, oldName, &ok);
    if (!ok || newName.isEmpty() || newName == oldName)
        return;

    QString base = currentRemotePath();
    if (!base.endsWith(QLatin1Char('/')))
        base += QLatin1Char('/');

    statusBar()->showMessage(QStringLiteral("Renaming ..."));
    QMetaObject::invokeMethod(worker_, "renameFile", Qt::QueuedConnection,
                              Q_ARG(QString, base + oldName),
                              Q_ARG(QString, base + newName));
}

void MainWindow::onMkdir()
{
    bool ok;
    QString name = QInputDialog::getText(this,
        QStringLiteral("New Folder"), QStringLiteral("Folder name:"),
        QLineEdit::Normal, QStringLiteral("new_folder"), &ok);
    if (!ok || name.isEmpty())
        return;

    QString path = currentRemotePath();
    if (!path.endsWith(QLatin1Char('/')))
        path += QLatin1Char('/');
    path += name;

    statusBar()->showMessage(QStringLiteral("Creating %1 ...").arg(path));
    QMetaObject::invokeMethod(worker_, "createDirectory", Qt::QueuedConnection,
                              Q_ARG(QString, path));
}

void MainWindow::onCancel()
{
    QMetaObject::invokeMethod(worker_, "cancelOperation", Qt::QueuedConnection);
    setBusy(false);
}

void MainWindow::onSelectionChanged()
{
    bool hasRow = fileTable_->currentRow() >= 0;
    bool conn = disconnectBtn_->isEnabled();  // connected state
    downloadBtn_->setEnabled(conn && hasRow);
    deleteBtn_->setEnabled(conn && hasRow);
    renameBtn_->setEnabled(conn && hasRow);
}

void MainWindow::onTableDoubleClicked(int row, int /*column*/)
{
    if (row < 0) return;

    bool isDir = fileTable_->item(row, 0)->data(Qt::UserRole).toBool();
    if (!isDir) return;

    QString path = currentRemotePath();
    if (!path.endsWith(QLatin1Char('/')))
        path += QLatin1Char('/');
    path += fileTable_->item(row, 0)->text();
    navigateTo(path);
}

// ---------------------------------------------------------------------------
// Worker signal handlers — these run in the UI thread
// ---------------------------------------------------------------------------

void MainWindow::onConnected()
{
    setConnectedState(true);
    statusBar()->showMessage(QStringLiteral("Connected"));
    navigateTo(QStringLiteral("/"));
}

void MainWindow::onDisconnected()
{
    setConnectedState(false);
    statusBar()->showMessage(QStringLiteral("Disconnected"));
}

void MainWindow::onConnectionError(const QString &error)
{
    setConnectedState(false);
    connectBtn_->setEnabled(true);
    statusBar()->showMessage(QStringLiteral("Error: %1").arg(error), 8000);
    QMessageBox::critical(this, QStringLiteral("Connection Error"), error);
}

void MainWindow::onDirectoryListed(const QString &path, const QList<RemoteFileInfo> &files)
{
    pathEdit_->setText(path);
    populateTable(files);
    if (fileTable_->rowCount() > 0) {
        fileTable_->selectRow(0);
    }
    onSelectionChanged();
    statusBar()->showMessage(QStringLiteral("%1 (%2 items)").arg(path).arg(files.size()));
}

void MainWindow::onDownloadProgress(const QString &remotePath, qint64 transferred, qint64 total)
{
    Q_UNUSED(remotePath);
    if (total > 0) {
        progressBar_->setRange(0, 100);
        progressBar_->setValue(static_cast<int>(transferred * 100 / total));
    }
    statusBar()->showMessage(
        QStringLiteral("Downloading: %1 / %2").arg(formatFileSize(transferred), formatFileSize(total)));
}

void MainWindow::onDownloadFinished(const QString &remotePath)
{
    setBusy(false);
    progressBar_->setValue(0);
    statusBar()->showMessage(
        QStringLiteral("Downloaded %1").arg(remotePath), 5000);
}

void MainWindow::onUploadProgress(const QString &localPath, qint64 transferred, qint64 total)
{
    Q_UNUSED(localPath);
    if (total > 0) {
        progressBar_->setRange(0, 100);
        progressBar_->setValue(static_cast<int>(transferred * 100 / total));
    }
    statusBar()->showMessage(
        QStringLiteral("Uploading: %1 / %2").arg(formatFileSize(transferred), formatFileSize(total)));
}

void MainWindow::onUploadFinished(const QString &localPath)
{
    setBusy(false);
    progressBar_->setValue(0);
    statusBar()->showMessage(
        QStringLiteral("Uploaded %1").arg(localPath), 5000);
    onRefresh();  // show new file
}

void MainWindow::onDeleteFinished(const QString &remotePath)
{
    statusBar()->showMessage(
        QStringLiteral("Done: %1").arg(remotePath), 5000);
    onRefresh();
}

void MainWindow::onOperationError(const QString &operation, const QString &error)
{
    setBusy(false);
    progressBar_->setValue(0);
    statusBar()->showMessage(
        QStringLiteral("%1 failed: %2").arg(operation, error), 8000);
    QMessageBox::warning(this,
        QStringLiteral("Operation Error"),
        QStringLiteral("%1: %2").arg(operation, error));
}
