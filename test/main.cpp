#include "test_log.h"

int main() {
        bool res(true);
        
        TestLogging logging;
        res &= logging.RunTest();
        
        return res ? 0 : 1;
}
