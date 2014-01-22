#include "test_log.h"

int main() {
        bool res(true);
        
        TestLogging logging;
        res &= logging.RunTest();

	if (res)
		logging.DeleteOutputFiles();
        
        return res ? 0 : 1;
}
