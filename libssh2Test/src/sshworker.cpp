// winsock2.h MUST come before anything that pulls in windows.h
#include <winsock2.h>
#include <ws2tcpip.h>

#include "sshworker.h"

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <QFile>
#include <QFileInfo>
#include <atomic>

// ---------------------------------------------------------------------------
// Private data — hides all platform / libssh2 types from the header
// ---------------------------------------------------------------------------

struct SshWorker::Private {
    SOCKET sock = INVALID_SOCKET;
    LIBSSH2_SESSION *session = nullptr;
    LIBSSH2_SFTP *sftp = nullptr;
    bool connected = false;
    std::atomic<bool> cancelled{false};
};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static QString formatPermissions(unsigned long perm)
{
    auto isDir  = (perm & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFDIR;
    auto isLnk  = (perm & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFLNK;

    auto r = [&](unsigned long mask) { return (perm & mask) ? 'r' : '-'; };
    auto w = [&](unsigned long mask) { return (perm & mask) ? 'w' : '-'; };
    auto x = [&](unsigned long mask) { return (perm & mask) ? 'x' : '-'; };

    return QString("%1%2%3%4%5%6%7%8%9%10")
        .arg(isDir ? 'd' : (isLnk ? 'l' : '-'))
        .arg(r(LIBSSH2_SFTP_S_IRUSR)).arg(w(LIBSSH2_SFTP_S_IWUSR)).arg(x(LIBSSH2_SFTP_S_IXUSR))
        .arg(r(LIBSSH2_SFTP_S_IRGRP)).arg(w(LIBSSH2_SFTP_S_IWGRP)).arg(x(LIBSSH2_SFTP_S_IXGRP))
        .arg(r(LIBSSH2_SFTP_S_IROTH)).arg(w(LIBSSH2_SFTP_S_IWOTH)).arg(x(LIBSSH2_SFTP_S_IXOTH));
}

static QString sftpErrorMessage(unsigned long err)
{
    switch (err) {
    case LIBSSH2_FX_OK:                return QStringLiteral("Success");
    case LIBSSH2_FX_EOF:               return QStringLiteral("End of file");
    case LIBSSH2_FX_NO_SUCH_FILE:      return QStringLiteral("No such file");
    case LIBSSH2_FX_PERMISSION_DENIED: return QStringLiteral("Permission denied");
    case LIBSSH2_FX_FAILURE:           return QStringLiteral("Failure");
    case LIBSSH2_FX_BAD_MESSAGE:       return QStringLiteral("Bad message");
    case LIBSSH2_FX_NO_CONNECTION:     return QStringLiteral("No connection");
    case LIBSSH2_FX_CONNECTION_LOST:   return QStringLiteral("Connection lost");
    case LIBSSH2_FX_OP_UNSUPPORTED:    return QStringLiteral("Operation unsupported");
    case LIBSSH2_FX_INVALID_HANDLE:    return QStringLiteral("Invalid handle");
    case LIBSSH2_FX_NO_SUCH_PATH:      return QStringLiteral("No such path");
    case LIBSSH2_FX_FILE_ALREADY_EXISTS: return QStringLiteral("File already exists");
    case LIBSSH2_FX_WRITE_PROTECT:     return QStringLiteral("Write protect");
    case LIBSSH2_FX_NO_MEDIA:          return QStringLiteral("No media");
    default:                           return QStringLiteral("Unknown error %1").arg(err);
    }
}

// ---------------------------------------------------------------------------
// SshWorker
// ---------------------------------------------------------------------------

SshWorker::SshWorker(QObject *parent)
    : QObject(parent)
    , p_(new Private)
{
}

SshWorker::~SshWorker()
{
    disconnectFromHost();
}

void SshWorker::cancelOperation()
{
    p_->cancelled = true;
}

// ---------------------------------------------------------------------------
// Connection
// ---------------------------------------------------------------------------

void SshWorker::connectToHost(const QString &host, quint16 port,
                               const QString &user, const QString &password)
{
    // 1. TCP socket
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        emit connectionError(QStringLiteral("WSAStartup failed"));
        return;
    }

    p_->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (p_->sock == INVALID_SOCKET) {
        emit connectionError(QStringLiteral("Failed to create socket"));
        WSACleanup();
        return;
    }

    struct addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *result = nullptr;
    if (getaddrinfo(host.toUtf8().constData(), nullptr, &hints, &result) != 0 || !result) {
        emit connectionError(QStringLiteral("Failed to resolve hostname"));
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    struct sockaddr_in addr;
    memcpy(&addr, result->ai_addr, sizeof(addr));
    addr.sin_port = htons(port);
    freeaddrinfo(result);

    if (::connect(p_->sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) != 0) {
        emit connectionError(QStringLiteral("TCP connection failed"));
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    // 2. libssh2 session
    p_->session = libssh2_session_init();
    if (!p_->session) {
        emit connectionError(QStringLiteral("libssh2_session_init failed"));
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    libssh2_session_set_blocking(p_->session, 1);

    if (libssh2_session_handshake(p_->session, p_->sock) != 0) {
        char *errmsg = nullptr;
        libssh2_session_last_error(p_->session, &errmsg, nullptr, 0);
        QString err = errmsg ? QString::fromLocal8Bit(errmsg) : QStringLiteral("unknown");
        emit connectionError(QStringLiteral("SSH handshake failed: %1").arg(err));
        libssh2_session_free(p_->session);
        p_->session = nullptr;
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    // 3. Authentication
    QByteArray userBytes = user.toUtf8();
    char *authList = libssh2_userauth_list(p_->session,
                                           userBytes.constData(),
                                           userBytes.length());

    if (!authList || !strstr(authList, "password")) {
        emit connectionError(QStringLiteral("Password auth not supported"));
        libssh2_session_free(p_->session);
        p_->session = nullptr;
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    QByteArray passBytes = password.toUtf8();
    if (libssh2_userauth_password(p_->session,
                                  userBytes.constData(),
                                  passBytes.constData()) != 0) {
        char *errmsg = nullptr;
        libssh2_session_last_error(p_->session, &errmsg, nullptr, 0);
        QString err = errmsg ? QString::fromLocal8Bit(errmsg) : QStringLiteral("unknown");
        emit connectionError(QStringLiteral("Authentication failed: %1").arg(err));
        libssh2_session_free(p_->session);
        p_->session = nullptr;
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    // 4. SFTP
    p_->sftp = libssh2_sftp_init(p_->session);
    if (!p_->sftp) {
        char *errmsg = nullptr;
        libssh2_session_last_error(p_->session, &errmsg, nullptr, 0);
        QString err = errmsg ? QString::fromLocal8Bit(errmsg) : QStringLiteral("unknown");
        emit connectionError(QStringLiteral("SFTP init failed: %1").arg(err));
        libssh2_session_free(p_->session);
        p_->session = nullptr;
        closesocket(p_->sock);
        WSACleanup();
        return;
    }

    p_->connected = true;
    emit connected();
}

void SshWorker::disconnectFromHost()
{
    p_->connected = false;

    if (p_->sftp) {
        libssh2_sftp_shutdown(p_->sftp);
        p_->sftp = nullptr;
    }
    if (p_->session) {
        libssh2_session_disconnect(p_->session, "bye");
        libssh2_session_free(p_->session);
        p_->session = nullptr;
    }
    if (p_->sock != INVALID_SOCKET) {
        closesocket(p_->sock);
        p_->sock = INVALID_SOCKET;
        WSACleanup();
    }

    emit disconnected();
}

// ---------------------------------------------------------------------------
// Directory listing
// ---------------------------------------------------------------------------

void SshWorker::listDirectory(const QString &path)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("list"), QStringLiteral("Not connected"));
        return;
    }

    p_->cancelled = false;

    QByteArray pathBytes = path.toUtf8();
    LIBSSH2_SFTP_HANDLE *dir = libssh2_sftp_opendir(p_->sftp, pathBytes.constData());
    if (!dir) {
        emit operationError(QStringLiteral("list"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    QList<RemoteFileInfo> files;
    char filename[512];
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    while (libssh2_sftp_readdir(dir, filename, sizeof(filename), &attrs) > 0) {
        if (p_->cancelled)
            break;

        QString name = QString::fromUtf8(filename);
        if (name == QLatin1String(".") || name == QLatin1String(".."))
            continue;

        RemoteFileInfo info;
        info.name = name;
        info.isDirectory = (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS)
                           && (LIBSSH2_SFTP_S_ISDIR(attrs.permissions));
        info.size = (attrs.flags & LIBSSH2_SFTP_ATTR_SIZE) ? attrs.filesize : 0;
        info.permissions = (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS)
                           ? formatPermissions(attrs.permissions) : QString();
        if (attrs.flags & LIBSSH2_SFTP_ATTR_ACMODTIME)
            info.modificationTime = QDateTime::fromSecsSinceEpoch(attrs.mtime);

        files.append(info);
    }

    libssh2_sftp_closedir(dir);

    // Sort: directories first, then by name
    std::sort(files.begin(), files.end(), [](const RemoteFileInfo &a, const RemoteFileInfo &b) {
        if (a.isDirectory != b.isDirectory)
            return a.isDirectory;
        return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
    });

    emit directoryListed(path, files);
}

// ---------------------------------------------------------------------------
// Download
// ---------------------------------------------------------------------------

void SshWorker::downloadFile(const QString &remotePath, const QString &localPath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("download"), QStringLiteral("Not connected"));
        return;
    }

    p_->cancelled = false;

    QByteArray rp = remotePath.toUtf8();
    LIBSSH2_SFTP_HANDLE *handle = libssh2_sftp_open(p_->sftp, rp.constData(),
                                                    LIBSSH2_FXF_READ, 0);
    if (!handle) {
        emit operationError(QStringLiteral("download"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    LIBSSH2_SFTP_ATTRIBUTES attrs;
    qint64 totalSize = 0;
    if (libssh2_sftp_fstat(handle, &attrs) == 0)
        totalSize = attrs.filesize;

    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly)) {
        libssh2_sftp_close(handle);
        emit operationError(QStringLiteral("download"),
                            QStringLiteral("Cannot write to %1").arg(localPath));
        return;
    }

    char buffer[64 * 1024];
    qint64 transferred = 0;
    ssize_t rc;

    while ((rc = libssh2_sftp_read(handle, buffer, sizeof(buffer))) > 0) {
        if (p_->cancelled)
            break;
        file.write(buffer, rc);
        transferred += rc;
        emit downloadProgress(remotePath, transferred, totalSize);
    }

    file.close();
    libssh2_sftp_close(handle);

    if (p_->cancelled)
        return;

    if (rc < 0)
        emit operationError(QStringLiteral("download"), QStringLiteral("Read error"));
    else
        emit downloadFinished(remotePath);
}

// ---------------------------------------------------------------------------
// Upload
// ---------------------------------------------------------------------------

void SshWorker::uploadFile(const QString &localPath, const QString &remotePath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("upload"), QStringLiteral("Not connected"));
        return;
    }

    p_->cancelled = false;

    QFile file(localPath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit operationError(QStringLiteral("upload"),
                            QStringLiteral("Cannot read %1").arg(localPath));
        return;
    }

    qint64 totalSize = file.size();

    QByteArray rp = remotePath.toUtf8();
    LIBSSH2_SFTP_HANDLE *handle = libssh2_sftp_open(p_->sftp, rp.constData(),
                                                    LIBSSH2_FXF_WRITE |
                                                    LIBSSH2_FXF_CREAT |
                                                    LIBSSH2_FXF_TRUNC,
                                                    LIBSSH2_SFTP_S_IRUSR |
                                                    LIBSSH2_SFTP_S_IWUSR |
                                                    LIBSSH2_SFTP_S_IRGRP |
                                                    LIBSSH2_SFTP_S_IROTH);
    if (!handle) {
        file.close();
        emit operationError(QStringLiteral("upload"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    char buffer[64 * 1024];
    qint64 transferred = 0;

    while (!file.atEnd()) {
        if (p_->cancelled)
            break;

        qint64 n = file.read(buffer, sizeof(buffer));
        ssize_t written = libssh2_sftp_write(handle, buffer, n);
        if (written < 0) {
            file.close();
            libssh2_sftp_close(handle);
            emit operationError(QStringLiteral("upload"), QStringLiteral("Write error"));
            return;
        }
        transferred += n;
        emit uploadProgress(localPath, transferred, totalSize);
    }

    file.close();
    libssh2_sftp_close(handle);

    if (p_->cancelled)
        return;

    emit uploadFinished(localPath);
}

// ---------------------------------------------------------------------------
// Delete file
// ---------------------------------------------------------------------------

void SshWorker::deleteFile(const QString &remotePath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("delete"), QStringLiteral("Not connected"));
        return;
    }

    QByteArray rp = remotePath.toUtf8();
    if (libssh2_sftp_unlink(p_->sftp, rp.constData()) != 0) {
        emit operationError(QStringLiteral("delete"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    emit deleteFinished(remotePath);
}

// ---------------------------------------------------------------------------
// Delete directory (must be empty)
// ---------------------------------------------------------------------------

void SshWorker::deleteDirectory(const QString &remotePath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("rmdir"), QStringLiteral("Not connected"));
        return;
    }

    QByteArray rp = remotePath.toUtf8();
    if (libssh2_sftp_rmdir(p_->sftp, rp.constData()) != 0) {
        emit operationError(QStringLiteral("rmdir"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    emit deleteFinished(remotePath);
}

// ---------------------------------------------------------------------------
// Rename
// ---------------------------------------------------------------------------

void SshWorker::renameFile(const QString &oldPath, const QString &newPath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("rename"), QStringLiteral("Not connected"));
        return;
    }

    QByteArray op = oldPath.toUtf8();
    QByteArray np = newPath.toUtf8();

    if (libssh2_sftp_rename(p_->sftp, op.constData(), np.constData()) != 0) {
        emit operationError(QStringLiteral("rename"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    emit deleteFinished(oldPath);  // reuse signal to trigger refresh
}

// ---------------------------------------------------------------------------
// Create directory
// ---------------------------------------------------------------------------

void SshWorker::createDirectory(const QString &remotePath)
{
    if (!p_->connected || !p_->sftp) {
        emit operationError(QStringLiteral("mkdir"), QStringLiteral("Not connected"));
        return;
    }

    QByteArray rp = remotePath.toUtf8();
    if (libssh2_sftp_mkdir(p_->sftp, rp.constData(),
                           LIBSSH2_SFTP_S_IRWXU |
                           LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IXGRP |
                           LIBSSH2_SFTP_S_IROTH | LIBSSH2_SFTP_S_IXOTH) != 0) {
        emit operationError(QStringLiteral("mkdir"),
                            sftpErrorMessage(libssh2_sftp_last_error(p_->sftp)));
        return;
    }

    emit deleteFinished(remotePath);  // reuse signal to trigger refresh
}
