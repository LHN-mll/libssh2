#pragma once

#include <QMainWindow>
#include <QThread>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include <QProgressBar>
#include <QStatusBar>

#include "sshworker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // UI actions
    void onConnect();
    void onDisconnect();
    void onRefresh();
    void onGoUp();
    void onGoHome();
    void onDownload();
    void onUpload();
    void onDelete();
    void onRename();
    void onMkdir();
    void onCancel();
    void onSelectionChanged();
    void onTableDoubleClicked(int row, int column);

    // Worker signals
    void onConnected();
    void onDisconnected();
    void onConnectionError(const QString &error);
    void onDirectoryListed(const QString &path, const QList<RemoteFileInfo> &files);
    void onDownloadProgress(const QString &remotePath, qint64 transferred, qint64 total);
    void onDownloadFinished(const QString &remotePath);
    void onUploadProgress(const QString &localPath, qint64 transferred, qint64 total);
    void onUploadFinished(const QString &localPath);
    void onDeleteFinished(const QString &remotePath);
    void onOperationError(const QString &operation, const QString &error);

private:
    void setupUi();
    void setConnectedState(bool connected);
    void setBusy(bool busy);
    void populateTable(const QList<RemoteFileInfo> &files);
    QString currentRemotePath() const;
    void navigateTo(const QString &path);

    // Connection widgets
    QLineEdit *hostEdit_   = nullptr;
    QLineEdit *portEdit_   = nullptr;
    QLineEdit *userEdit_   = nullptr;
    QLineEdit *passwordEdit_ = nullptr;
    QPushButton *connectBtn_    = nullptr;
    QPushButton *disconnectBtn_ = nullptr;

    // Path
    QLineEdit *pathEdit_  = nullptr;
    QPushButton *homeBtn_ = nullptr;
    QPushButton *upBtn_   = nullptr;
    QPushButton *refreshBtn_ = nullptr;

    // File table
    QTableWidget *fileTable_ = nullptr;

    // Operations
    QPushButton *downloadBtn_ = nullptr;
    QPushButton *uploadBtn_   = nullptr;
    QPushButton *deleteBtn_   = nullptr;
    QPushButton *renameBtn_   = nullptr;
    QPushButton *mkdirBtn_    = nullptr;
    QPushButton *cancelBtn_   = nullptr;

    // Progress
    QProgressBar *progressBar_ = nullptr;

    // Thread & Worker
    QThread    *workerThread_ = nullptr;
    SshWorker  *worker_       = nullptr;
};
