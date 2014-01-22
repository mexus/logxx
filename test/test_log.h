/* 
 * File:   test_log.h
 * Author: mexus
 *
 * Created on January 21, 2014, 3:33 PM
 */

#ifndef TEST_LOG_H
#define	TEST_LOG_H

#include <iostream>
#include <fstream>
#include <vector>

#define protected public
#include "../main/log.h"
#undef protected

using namespace logxx;

class TestLogging {
public:
        TestLogging();
        virtual ~TestLogging();
        
        bool RunTest();
        
private:
        static Log cLog;
        static const std::string dataDir, outputPrefix, etalonPrefix;
        
        std::streambuf *oldOutBuf = nullptr;
        std::ofstream outFile;
        
        std::vector<std::string> outputFiles;
        
        bool TestRedirection();
        
        bool TestLevels();
        void WriteGlobalLevel(Log &) const;
        void WriteLocalLevel(Log &) const;
        void WriteInheritedLevel(Log &) const;
        
        void PrintLevels(Log &) const;
        
        bool CompareFiles(const std::string& name) const;
};

#endif	/* TEST_LOG_H */

