#include "worker.h"
#include <QDebug>

Worker::Worker() : listening(false) {}

Worker::~Worker() {
    if (listening) {
        stopListening();
    }
}

void Worker::setupConnection() {
    // Connect to RabbitMQ
    conn = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(conn);
    int status = amqp_socket_open(socket, "remote.runflare.com", 31852);
    if (status) {
        emit logMessage("Error opening TCP socket.");
        return;
    }

    amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "root", "bFunufNwEV3aVKx2Wf3k");
    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    amqp_basic_consume(conn, 1, amqp_cstring_bytes("NodeJsToCpp"), amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
    amqp_get_rpc_reply(conn);

    emit logMessage("RabbitMQ connected and listening to 'NodeJsToCpp'.");
}

void Worker::listenToQueue() {
    listening = true;

    while (listening) {
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(conn);
        res = amqp_consume_message(conn, &envelope, NULL, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            emit logMessage("Error consuming message from RabbitMQ.");
            break;
        }

        // Convert the message body to QString
        QString message = QString::fromStdString(std::string((char*)envelope.message.body.bytes, envelope.message.body.len));

        emit messageReceived(message);  // Emit the message to the main thread

        amqp_destroy_envelope(&envelope);
    }

    // Clean up the connection
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

    emit logMessage("RabbitMQ connection closed.");
}

void Worker::stopListening() {
    listening = false;
}

void Worker::sendMessage(const QString &message) {
    amqp_basic_publish(conn, 1, amqp_cstring_bytes(""), amqp_cstring_bytes("CppToNodeJs"), 0, 0, NULL, amqp_cstring_bytes(message.toStdString().c_str()));
    emit logMessage("Message sent to RabbitMQ: " + message);
}
