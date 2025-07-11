#include "gmock/gmock.h"
#include "krado/log.h"

int
main(int argc, char ** argv)
{
    krado::Log::set_verbosity(0);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
