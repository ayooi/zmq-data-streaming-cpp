//
// Created by arans on 3/06/2021.
//
#include <iostream>
#include <zmq.h>

int main(int argc, char **argv) {
    std::cout << "Hello World!" << std::endl;
    auto ctx = zmq_ctx_new();
    auto push = zmq_socket(ctx, ZMQ_PUSH);
    zmq_bind(push, "inproc://testest");

    auto pull = zmq_socket(ctx, ZMQ_PULL);
    zmq_connect(pull, "inproc://testest");

    {
        zmq_send(push, "Hello", 5, 0);
    }

    {
        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_recvmsg(pull, &msg, 0);
        std::cout << std::string(static_cast<char*>(zmq_msg_data(&msg)), zmq_msg_size(&msg)) << std::endl;
    }

    zmq_close(push);
    zmq_close(pull);
    zmq_ctx_destroy(ctx);
    return 0;
}