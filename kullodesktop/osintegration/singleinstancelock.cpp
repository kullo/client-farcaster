/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "singleinstancelock.h"

#include <cstdint>

#include <QLocalServer>
#include <QLocalSocket>

#include <kulloclient/util/librarylogger.h>
#include <kulloclient/util/assert.h>
#include <desktoputil/qtypestreamers.h>

namespace KulloDesktop {
namespace OsIntegration {

namespace {
const auto MESSAGE_SIZE_LIMIT = 50; // bytes
const auto MESSAGE_GO_TO_FOREGROUND_REQUEST = "go to foreground";
const auto MESSAGE_GO_TO_FOREGROUND_ACCEPT = "ok";

auto LOCAL_SOCKET_ERROR_STRINGS = std::map<QLocalSocket::LocalSocketError, std::string>{
    {QLocalSocket::ConnectionRefusedError, "ConnectionRefusedError"},
    {QLocalSocket::PeerClosedError, "PeerClosedError"},
    {QLocalSocket::ServerNotFoundError, "ServerNotFoundError"},
    {QLocalSocket::SocketAccessError, "SocketAccessError"},
    {QLocalSocket::SocketResourceError, "SocketResourceError"},
    {QLocalSocket::SocketTimeoutError, "SocketTimeoutError"},
    {QLocalSocket::DatagramTooLargeError, "DatagramTooLargeError"},
    {QLocalSocket::ConnectionError, "ConnectionError"},
    {QLocalSocket::UnsupportedSocketOperationError, "UnsupportedSocketOperationError"},
    {QLocalSocket::OperationError, "OperationError"},
    {QLocalSocket::UnknownSocketError, "UnknownSocketError"},
};
}

SingleInstanceLock::SingleInstanceLock(const QString &name, QObject *parent)
    : QObject(parent)
    , name_(name)
{
}

SingleInstanceLock::~SingleInstanceLock()
{
}

SingleInstanceLock::AcquireResult SingleInstanceLock::acquire()
{
    auto connection = connectToServer();

    if (connection)
    {
        connection->write(MESSAGE_GO_TO_FOREGROUND_REQUEST);
        connection->flush();

        if (connection->waitForReadyRead(100))
        {
            auto readResult = connection->read(MESSAGE_SIZE_LIMIT);
            auto message = QString::fromUtf8(readResult);

            Log.d() << "Received message: " << message;

            if (message == MESSAGE_GO_TO_FOREGROUND_ACCEPT)
            {
                return AcquireResult::InformedOtherInstance;
            }
            else
            {
                Log.f() << "Received invalid message.";

                // silence "control may reach end of non-void function" warning
                return AcquireResult::InformedOtherInstance;
            }
        }
        else
        {
            Log.e() << "Socket server did not answer.";
            purgeBrokenServer();
            startServer();
            return AcquireResult::CreatedInstance;
        }
    }
    else
    {
        purgeBrokenServer();
        startServer();
        return AcquireResult::CreatedInstance;
    }
}

void SingleInstanceLock::release()
{
    if (server_)
    {
        server_->close();
    }
}

std::unique_ptr<QLocalSocket> SingleInstanceLock::connectToServer()
{
    auto socket = Kullo::make_unique<QLocalSocket>();
    socket->connectToServer(name_);
    if (socket->waitForConnected(500))
    {
        return socket;
    }
    else
    {
        // Do not warn because this case is normal for the first instance
        Log.d() << "Could not connect to SingleInstanceLock server: "
                << LOCAL_SOCKET_ERROR_STRINGS[socket->error()];
        return nullptr;
    }
}

void SingleInstanceLock::purgeBrokenServer()
{
    QLocalServer::removeServer(name_);
}

void SingleInstanceLock::startServer()
{
    // server does not exist yet
    server_ = Kullo::make_unique<QLocalServer>(nullptr);
    auto ret = server_->listen(name_);
    kulloAssert(ret == true);

    connect(server_.get(), &QLocalServer::newConnection,
            this, [this]() {
        QLocalSocket *socketConnection = server_->nextPendingConnection(); // auto-deleted with server
        connect(socketConnection, &QLocalSocket::readyRead,
                this, [socketConnection, this]() {
            auto message = QString::fromUtf8(socketConnection->read(MESSAGE_SIZE_LIMIT));

            if (message == MESSAGE_GO_TO_FOREGROUND_REQUEST)
            {
                Log.d() << "Other process asked me to go to foreground.";
                emit showMainWindowRequested();
                socketConnection->write(MESSAGE_GO_TO_FOREGROUND_ACCEPT);
            }
            else
            {
                Log.f() << "Received invalid message.";
            }
        });
    });
}

}
}
