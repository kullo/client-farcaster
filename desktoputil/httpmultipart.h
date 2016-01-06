/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <cstdint>
#include <string>
#include <sstream>

namespace DesktopUtil {

class HttpMultipart
{
public:
    HttpMultipart(const std::string &boundary = "boundary_.oOo._MTA1NTM3NjEwMg==Mjc5OTIzODY0MTI0MTc2NjY=");
    std::string boundary() const;

    std::string makeTextPart(const std::string &name, const std::string &data);
    std::string makeTextPart(const std::string &name, std::uint32_t data);
    std::string makeFilePart(const std::string &name, const std::string &path);
    void addTextPart(const std::string &name, const std::string &data);
    void addTextPart(const std::string &name, std::uint32_t data);
    void addFilePart(const std::string &name, const std::string &path);
    std::string toString();

private:
    std::string boundary_;
    std::stringstream out_;
    bool finalized_ = false;
};

}
