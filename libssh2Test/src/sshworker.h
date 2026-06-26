#pragma once

#include <QObject>
#include <QDateTime>
#include <QList>
#include <QScopedPointer>

struct RemoteFileInfo {
    QString name;
    bool isDirectory = false;
    qint64 size = 0;
    QDateTime modificationTime;
    QString permissions;
};

class SshWorker : public QObject {
    Q_OBJECT
public:
    explicit SshWorker(QObject *parent = nullptr);
    ~SshWorker();

public slots:
    void connectToHost(const QString &host, quint16 port,
                       const QString &user, const QString &password);
    void disconnectFromHost();
    void listDirectory(const QString &path);
    void downloadFile(const QString &remotePath, const QString &localPath);
    void uploadFile(const QString &localPath, const QString &remotePath);
    void deleteFile(const QString &remotePath);
    void deleteDirectory(const QString &remotePath);
    void renameFile(const QString &oldPath, const QString &newPath);
    void createDirectory(const QString &remotePath);
    void cancelOperation();

signals:
    void connected();
    void disconnected();
    void connectionError(const QString &error);
    void directoryListed(const QString &path, const QList<RemoteFileInfo> &files);
    void downloadProgress(const QString &remotePath, qint64 transferred, qint64 total);
    void downloadFinished(const QString &remotePath);
    void uploadProgress(const QString &localPath, qint64 transferred, qint64 total);
    void uploadFinished(const QString &localPath);
    void deleteFinished(const QString &remotePath);
    void operationError(const QString &operation, const QString &error);

private:
    struct Private;
    QScopedPointer<Private> p_;
};

Q_DECLARE_METATYPE(RemoteFileInfo)
Q_DECLARE_METATYPE(QList<RemoteFileInfo>)
