/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "httpmultipart.h"

#include <kulloclient/util/assert.h>
#include <kulloclient/util/binary.h>
#include <kulloclient/util/filesystem.h>

namespace DesktopUtil {

HttpMultipart::HttpMultipart(const std::string &boundary)
    : boundary_(boundary)
{
}

std::string HttpMultipart::boundary() const
{
    return boundary_;
}

std::string HttpMultipart::makeTextPart(const std::string &name, const std::string &data)
{
    std::stringstream out;
    out << "--" << boundary_ << "\r\n"
        << "Content-Disposition: form-data; name=\"" << name << "\"\r\n"
        << "\r\n"
        << data << "\r\n";
    return out.str();
}

std::string HttpMultipart::makeTextPart(const std::string &name, std::uint32_t data)
{
    return makeTextPart(name, std::to_string(data));
}

std::string HttpMultipart::makeFilePart(const std::string &name, const std::string &path)
{
    auto filename = Kullo::Util::Filesystem::filename(path);
    auto data = Kullo::Util::Filesystem::getContent(path);

    std::stringstream out;
    out << "--" << boundary_ << "\r\n"
        << "Content-Disposition: form-data; name=\"" << name << "\"; filename=\"" << filename << "\"\r\n"
        << "Content-Type: application/octet-stream\r\n"
        << "\r\n"
        << Kullo::Util::to_string(data) << "\r\n";
    return out.str();
}

void HttpMultipart::addTextPart(const std::string &name, const std::string &data)
{
    kulloAssert(!finalized_);
    out_ << makeTextPart(name, data);
}

void HttpMultipart::addTextPart(const std::string &name, std::uint32_t data)
{
    kulloAssert(!finalized_);
    out_ << makeTextPart(name, data);
}

void HttpMultipart::addFilePart(const std::string &name, const std::string &path)
{
    kulloAssert(!finalized_);
    out_ << makeFilePart(name, path);
}

std::string HttpMultipart::toString()
{
    if (!finalized_)
    {
        out_ << "--" << boundary_ << "--\r\n";
        finalized_ = true;
    }

    return out_.str();
}

}
