#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker();
    ~Worker();

    void setupConnection();
    void listenToQueue();
    void sendMessage(const QString &message);

public slots:
    void stopListening();

signals:
    void messageReceived(const QString &message);
    void logMessage(const QString &message);

private:
    amqp_connection_state_t conn;
    bool listening;
};

#endif // WORKER_H
