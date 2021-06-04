//
// Created by arans on 4/06/2021.
//

#include <gtest/gtest.h>
#include "DataServiceReader.h"
#include <zmq.h>
#include <chrono>

using namespace std::chrono_literals;

namespace dataservicereadertest {
    TEST(DataServiceReaderTest, basicRead) {
        auto ctx = zmq_ctx_new();
        auto serviceLocatorUrl = "inproc://service-locator";
        auto dataUrl = "inproc://data-url";

        auto router = zmq_socket(ctx, ZMQ_ROUTER);
        zmq_bind(router, serviceLocatorUrl);

        auto push = zmq_socket(ctx, ZMQ_PUSH);
        zmq_bind(push, dataUrl);

        DataServiceReader reader("unique-name",
                                 "service-name",
                                 ctx,
                                 serviceLocatorUrl);

        zmq_pollitem_t pollitems[] = {
                {router, 0, ZMQ_POLLIN, 0}
        };

        // grab and hold the address message
        std::string address;
        zmq_msg_t addressMsg;
        zmq_msg_init(&addressMsg);
        {
            int rc = zmq_poll(pollitems, 1, 0);
            ASSERT_NE(-1, rc);
            ASSERT_TRUE(pollitems[0].revents & ZMQ_POLLIN);
            zmq_msg_recv(&addressMsg, router, 0);
        }
        // expect a 'query' command here
        {
            int rc = zmq_poll(pollitems, 1, 0);
            ASSERT_NE(-1, rc);
            ASSERT_TRUE(pollitems[0].revents & ZMQ_POLLIN);

            zmq_msg_t msgs;
            zmq_msg_init(&msgs);
            zmq_msg_recv(&msgs, router, 0);
            auto command = std::string(static_cast<char *>(zmq_msg_data(&msgs)), zmq_msg_size(&msgs));
            ASSERT_EQ("query", command);
            zmq_msg_close(&msgs);
        }
        // expect the service name here
        {
            int rc = zmq_poll(pollitems, 1, 0);
            ASSERT_NE(-1, rc);
            ASSERT_TRUE(pollitems[0].revents & ZMQ_POLLIN);

            zmq_msg_t msgs;
            zmq_msg_init(&msgs);
            zmq_msg_recv(&msgs, router, 0);
            auto serviceName = std::string(static_cast<char *>(zmq_msg_data(&msgs)), zmq_msg_size(&msgs));
            ASSERT_EQ("service-name", serviceName);
            zmq_msg_close(&msgs);
        }

        {
            // send back data location
            zmq_msg_send(&addressMsg, router, ZMQ_SNDMORE);
            zmq_send(router, dataUrl, strlen(dataUrl), 0);
            zmq_msg_close(&addressMsg);
        }

        std::string payload = "Payload";
        zmq_send(push, payload.c_str(), payload.length(), 0);

        std::this_thread::sleep_for(50ms);

        std::list<std::vector<uint8_t>> res;
        reader.takeAll(res);

        ASSERT_EQ(1, res.size());
        auto vec = res.front();
        auto resString = std::string(vec.data(), vec.data() + vec.size());
        ASSERT_EQ(payload, resString);
    }

}