/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#include "../kullodesktoptest.h"

#include <desktoputil/initials.h>

using namespace testing;

class Initials : public KulloDesktopTest
{
};

TEST_F(Initials, data)
{
    const auto data = std::vector<QString>{
            // empty
            "", "",
            // simple
            "John", "J",
            "John Doe", "JD",
            // compound names
            "Foo Abc-Def", "FA",
            "Foo de Abc-Def", "FA",
            "Foo -Bar", "FB",
            // lower case
            "john", "J",
            "john doe", "JD",
            // limit length
            "John Mike Doe", "JD",
            "John Mike Doe Junior", "JJ",
            // numbers are okay
            "John 2", "J2",
            "1 2", "12",
            "4 All", "4A",
            // ignore special characters
            "John (Tablet)", "JT",
            "John _Tablet", "JT",
            "John 🐵 Doe", "JD",
            "John 🐵Doe", "JD",
            "🐵", "",
            // ignore empty parts
            "John ", "J",
            "John _", "J",
            // different whitespace
            "John  Doe", "JD",
            "John\tDoe", "JD",
            "John\nDoe", "JD",
            "John\rDoe", "JD",
            "John\t Doe", "JD",
            "John\n Doe", "JD",
            "John\r Doe", "JD",
            "John \tDoe", "JD",
            "John \nDoe", "JD",
            "John \r Doe", "JD",
            "John \t Doe", "JD",
            "John \n Doe", "JD",
            "John \r Doe", "JD",
            // Umlauts work
            "La Österreich", "LÖ",
            "ὕδωρ ῥυθμός", "ὝῬ",
            // Three bytes
            "朽", "朽",
            "A 朽", "A朽",
            "朽 A", "朽A",
            // Non-BMP character (U+01D49E)
            "𝒞", "𝒞",
            "A 𝒞", "A𝒞",
            "𝒞 A", "𝒞A",
    };

    for (auto itr = data.cbegin(); itr != data.cend(); itr += 2)
    {
        const auto in = *itr;
        const auto expected = *(itr + 1);
        EXPECT_THAT(DesktopUtil::Initials::fromName(in), Eq(expected));
    }
}
