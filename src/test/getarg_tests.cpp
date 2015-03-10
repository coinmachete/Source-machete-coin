#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-MTC");
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", false));
    BOOST_CHECK(GetBoolArg("-MTC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-MTCo"));
    BOOST_CHECK(!GetBoolArg("-MTCo", false));
    BOOST_CHECK(GetBoolArg("-MTCo", true));

    ResetArgs("-MTC=0");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", false));
    BOOST_CHECK(!GetBoolArg("-MTC", true));

    ResetArgs("-MTC=1");
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", false));
    BOOST_CHECK(GetBoolArg("-MTC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noMTC");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", false));
    BOOST_CHECK(!GetBoolArg("-MTC", true));

    ResetArgs("-noMTC=1");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", false));
    BOOST_CHECK(!GetBoolArg("-MTC", true));

    ResetArgs("-MTC -noMTC");  // -MTC should win
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", false));
    BOOST_CHECK(GetBoolArg("-MTC", true));

    ResetArgs("-MTC=1 -noMTC=1");  // -MTC should win
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", false));
    BOOST_CHECK(GetBoolArg("-MTC", true));

    ResetArgs("-MTC=0 -noMTC=0");  // -MTC should win
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", false));
    BOOST_CHECK(!GetBoolArg("-MTC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--MTC=1");
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", false));
    BOOST_CHECK(GetBoolArg("-MTC", true));

    ResetArgs("--noMTC=1");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", false));
    BOOST_CHECK(!GetBoolArg("-MTC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTC", "eleven"), "eleven");

    ResetArgs("-MTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTC", "eleven"), "");

    ResetArgs("-MTC=");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTC", "eleven"), "");

    ResetArgs("-MTC=11");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-MTC", "eleven"), "11");

    ResetArgs("-MTC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-MTC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MTC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-MTC", 0), 0);

    ResetArgs("-MTC -bar");
    BOOST_CHECK_EQUAL(GetArg("-MTC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-MTC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-MTC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-MTC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-MTC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--MTC");
    BOOST_CHECK_EQUAL(GetBoolArg("-MTC"), true);

    ResetArgs("--MTC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-MTC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noMTC");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", true));
    BOOST_CHECK(!GetBoolArg("-MTC", false));

    ResetArgs("-noMTC=1");
    BOOST_CHECK(!GetBoolArg("-MTC"));
    BOOST_CHECK(!GetBoolArg("-MTC", true));
    BOOST_CHECK(!GetBoolArg("-MTC", false));

    ResetArgs("-noMTC=0");
    BOOST_CHECK(GetBoolArg("-MTC"));
    BOOST_CHECK(GetBoolArg("-MTC", true));
    BOOST_CHECK(GetBoolArg("-MTC", false));

    ResetArgs("-MTC --noMTC");
    BOOST_CHECK(GetBoolArg("-MTC"));

    ResetArgs("-noMTC -MTC"); // MTC always wins:
    BOOST_CHECK(GetBoolArg("-MTC"));
}

BOOST_AUTO_TEST_SUITE_END()
