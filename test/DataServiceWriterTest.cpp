//
// Created by arans on 4/06/2021.
//

#include <gtest/gtest.h>
#include "DataServiceWriter.h"
#include <zmq.h>

namespace dataservicewritertest {
    TEST(DataServiceWriterTest, basicWrite) {
        auto ctx = zmq_ctx_new();
        const char *dataUrl = "tcp://192.168.0.214:12947";
        DataServiceWriter writer("service-name",
                                 parse(dataUrl),
                                 ctx,
                                 "inproc://service-locator");

        auto pull = zmq_socket(ctx, ZMQ_PULL);
        zmq_connect(pull, dataUrl);

        std::string payload = "Hello";
        writer.write(reinterpret_cast<const uint8_t *>(payload.c_str()), payload.length());

        zmq_msg_t msg;
        zmq_msg_init(&msg);
        zmq_msg_recv(&msg, pull, 0);

        auto result = std::string(static_cast<char *>(zmq_msg_data(&msg)), zmq_msg_size(&msg));

        zmq_msg_close(&msg);

        ASSERT_EQ(payload, result);
    }

}