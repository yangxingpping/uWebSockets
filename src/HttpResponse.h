#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

/* An HttpResponse is the channel on which you send back a response */

#include "AsyncSocket.h"
#include "HttpResponseData.h"

namespace uWS {

/* Some pre-defined status constants to use with writeStatus */
const char *HTTP_200_OK = "200 OK";

template <bool SSL>
struct HttpResponse : public AsyncSocket<SSL> {
private:
    HttpResponseData<SSL> *getHttpResponseData() {
        return (HttpResponseData<SSL> *) AsyncSocket<SSL>::getExt();
    }

    int u32toa(uint32_t value, char *dst) {
        char temp[10];
        char *p = temp;
        do {
            *p++ = (char) (value % 10) + '0';
            value /= 10;
        } while (value > 0);

        int ret = p - temp;

        do {
            *dst++ = *--p;
        } while (p != temp);

        return ret;
    }

    /* Write an unsigned 32-bit integer */
    void writeUnsigned(unsigned int value) {
        char buf[10];
        int length = u32toa(value, buf);

        /* For now we do this copy */
        AsyncSocket<SSL>::write(buf, length);
    }

public:
    /* Write the HTTP status */
    HttpResponse *writeStatus(std::string_view status) {
        AsyncSocket<SSL>::write("HTTP/1.1 ", 9);
        AsyncSocket<SSL>::write(status.data(), status.length());
        AsyncSocket<SSL>::write("\r\n", 2);
        return this;
    }

    /* Write an HTTP header with string value */
    HttpResponse *writeHeader(std::string_view key, std::string_view value) {
        AsyncSocket<SSL>::write(key.data(), key.length());
        AsyncSocket<SSL>::write(": ", 2);
        AsyncSocket<SSL>::write(value.data(), value.length());
        AsyncSocket<SSL>::write("\r\n", 2);
        return this;
    }

    /* Attach a write handler for sending data. Length must be specified up front and chunks might be read more than once */
    void write(std::function<std::string_view(int)> cb, int length) {
        std::string_view chunk = cb(0);
        AsyncSocket<SSL>::write("Content-Length: ", 16);
        writeUnsigned(chunk.length());
        AsyncSocket<SSL>::write("\r\n\r\n", 4);
        if (int written; (written = AsyncSocket<SSL>::write(chunk.data(), chunk.length(), true)) < length) {
            std::cout << "HttpResponse::write failed to write everything" << std::endl;
            getHttpResponseData()->offset = written;
            getHttpResponseData()->outStream = cb;
        }
    }

    /* Convenience function for static data (I don't like this one!) */
    void write(std::string_view data) {
        write([data](int offset) {
            return data.substr(offset);
        }, data.length());
    }

    /* Attach a read handler for data sent. Will be called with a chunk of size 0 when FIN */
    void read(std::function<void(std::string_view)> handler) {
        HttpResponseData<SSL> *data = getHttpResponseData();
        data->inStream = handler;
    }
};

}

#endif // HTTPRESPONSE_H
