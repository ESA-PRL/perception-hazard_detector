#include <boost/test/unit_test.hpp>
#include <hazard_detection/Dummy.hpp>

using namespace hazard_detection;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    hazard_detection::DummyClass dummy;
    dummy.welcome();
}
