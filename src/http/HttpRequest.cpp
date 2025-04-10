#include "HttpRequest.h"

#include <sstream>

HttpRequest::HttpRequest(const std::string &raw_request) {
    ParseRequest(raw_request);
}

void HttpRequest::ParseRequest(const std::string &raw_request) {
    std::istringstream stream(raw_request);
    std::string line;

    if (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        line_stream >> method_ >> path_ >> version_;
    }

    while (std::getline(stream, line) && !line.empty()) {
    
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) break; 

        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2); 
            headers_[key] = value;
        }
    }

    if (stream.peek() == '\r') stream.get();
    if (stream.peek() == '\n') stream.get();

    if (stream.rdbuf()->in_avail() > 0) {
        std::ostringstream body_stream;
        body_stream << stream.rdbuf();
        body_ = body_stream.str();
    }

}


std::string HttpRequest::GetMethod() const {
    return method_;
}

std::string HttpRequest::GetPath() const {
    return path_;
}

std::string HttpRequest::GetVersion() const {
    return version_;
}

std::string HttpRequest::GetHeader(const std::string &key) const {
    const auto it = headers_.find(key);
    return (it != headers_.end()) ? it->second : "";
}

std::string HttpRequest::GetBody() const {
    return body_;
}
