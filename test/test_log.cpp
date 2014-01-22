/*
 * File:   test_log.cpp
 * Author: mexus
 * 
 * Created on January 21, 2014, 3:33 PM
 */

#include "test_log.h"
#include "config.h"
#include <cstdio>

Log TestLogging::cLog("TestLogging");

#ifndef DATA_DIR
#error "DATA_DIR should be set"
#endif //DATA_DIR

const std::string TestLogging::dataDir(DATA_DIR);
const std::string TestLogging::etalonPrefix(".etalon");
const std::string TestLogging::outputPrefix(".output");

TestLogging::TestLogging() {
}

TestLogging::~TestLogging() {
        S_LOG("~TestLogging");
        for (const std::string &name : outputFiles){
                std::string fullName = dataDir + name + outputPrefix;
                if (std::remove(fullName.c_str()) != 0)
                        log(Log::warning) << "Unable to delete file {" << fullName << "}" << Log::endl;
        }
}

bool TestLogging::TestRedirection() {
        S_LOG("TestRedirection");
        std::string outFilename = dataDir + "redirect" + outputPrefix;
        outputFiles.push_back("redirect");
        
        log.OverrideStream(outFilename);
        log(Log::info) << "Testing redirection" << Log::endl;
        
        return CompareFiles("redirect");
}

bool TestLogging::TestLevels() {
        S_LOG("TestLevels");
        bool res(true);
        
        log.OverrideStream(dataDir + "global" + outputPrefix);
        WriteGlobalLevel(log);
        res &= CompareFiles("global");
        
        log.OverrideStream(dataDir + "local" + outputPrefix);
        WriteLocalLevel(log);
        res &= CompareFiles("local");
        
        log.OverrideStream(dataDir + "inheritLevel" + outputPrefix);
        WriteInheritedLevel(log);
        res &= CompareFiles("inheritLevel");
        
        outputFiles.insert(outputFiles.end(), {"global", "local", "inheritLevel"});
        
        return res;
}

void TestLogging::WriteInheritedLevel(Log& log) const {
        log.OverrideLogLevel(Log::debug);
        Log log2(log, "WriteInheritedLevel");
        log.OverrideLogLevel(Log::error);
        PrintLevels(log2);
}

void TestLogging::WriteGlobalLevel(Log& l) const {
        Log log(l, "WriteGlobalLevel");
#define LVL(level) Log::SetLogLevel(level); log(Log::nothing) << "Level set to " #level << Log::endl; PrintLevels(log);
        LVL(Log::debug);
        LVL(Log::notice);
        LVL(Log::warning);
        LVL(Log::info);
        LVL(Log::error);
        LVL(Log::nothing);
#undef LVL
        Log::SetLogLevel(Log::debug);
}

void TestLogging::WriteLocalLevel(Log& l) const {
        Log log(l, "WriteLocalLevel");
#define LVL(level) log.OverrideLogLevel(level); log(Log::nothing) << "Level set to " #level << Log::endl; PrintLevels(log);
        LVL(Log::debug);
        LVL(Log::notice);
        LVL(Log::warning);
        LVL(Log::info);
        LVL(Log::error);
        LVL(Log::nothing);
#undef LVL
        Log::SetLogLevel(Log::debug);
}

void TestLogging::PrintLevels(Log& log) const {
        Log currentLog(log, "PrintLevels");
        currentLog(Log::debug)   << Log::endl;
        currentLog(Log::notice)  << Log::endl;
        currentLog(Log::warning) << Log::endl;
        currentLog(Log::info)    << Log::endl;
        currentLog(Log::error)   << Log::endl;
}

bool TestLogging::CompareFiles(const std::string& name) const {
        S_LOG("CompareFiles");
        std::string output = dataDir + name + outputPrefix;
        std::string original = dataDir + name + etalonPrefix;
        std::ifstream res(output), etalon(original);
        if (res.good() && etalon.good()){
                while (res.good() && etalon.good()){
                        auto c1 = res.get();
                        auto c2 = etalon.get();
                        if (res.good() && !etalon.good()){
                                log(Log::error) << "Problem while reading etalon file at " << res.tellg() << Log::endl;
                                return false;
                        } else if (!res.good() && etalon.good()){
                                log(Log::error) << "Problem while reading result file at " << etalon.tellg() << Log::endl;
                                return false;
                        } else if (!res.good() || !etalon.good())
                                break;
                        if (c1 != c2){
                                log(Log::error) << "At " << etalon.tellg() << ": [" << (char)c1 << "] != [" << (char)c2 << "]" << Log::endl;
                                return false;
                        }
                }
                return true;
        } else {
                log(Log::error) << "Can't open files for comparison: {" << output << "} and {" << original << "}" <<
                        Log::endl;
                return false;
        }
}

bool TestLogging::RunTest() {
        S_LOG("RunTest");
        bool res;
        Log::SetLogLevel(Log::debug);
        log(Log::info) << "Running tests" << Log::endl;
        if (TestRedirection()){
                res = TestLevels();
        } else
                res = false;
        log.OverrideStream();
        if (res)
                log(Log::info) << "OK" << Log::endl;
        else
                log(Log::info) << "FAILED" << Log::endl;
        return res;
}

