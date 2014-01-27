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
}

void TestLogging::DeleteOutputFiles(){
        S_LOG("DeleteOutputFiles");
        for (const std::string &name : outputFiles){
                std::string fullName = dataDir + name + outputPrefix;
                if (std::remove(fullName.c_str()) != 0)
                        log(logxx::warning) << "Unable to delete file {" << fullName << "}" << Log::endl;
        }
	outputFiles.clear();
}

bool TestLogging::TestRedirection() {
        S_LOG("TestRedirection");
        std::string outFilename = dataDir + "redirect" + outputPrefix;
        outputFiles.push_back("redirect");
        
        log.OverrideStream(outFilename);
        log(logxx::info) << "Testing redirection" << Log::endl;
        
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
        log.OverrideLogLevel(logxx::debug);
        Log log2(log, "WriteInheritedLevel");
        log.OverrideLogLevel(logxx::error);
        PrintLevels(log2);
}

void TestLogging::WriteGlobalLevel(Log& l) const {
        Log log(l, "WriteGlobalLevel");
#define LVL(level) Log::GlobalLogLevel(level); log(logxx::nothing) << "Level set to " #level << Log::endl; PrintLevels(log);
        LVL(logxx::debug);
        LVL(logxx::notice);
        LVL(logxx::warning);
        LVL(logxx::info);
        LVL(logxx::error);
        LVL(logxx::nothing);
#undef LVL
        Log::GlobalLogLevel(logxx::debug);
}

void TestLogging::WriteLocalLevel(Log& l) const {
        Log log(l, "WriteLocalLevel");
#define LVL(level) log.OverrideLogLevel(level); log(logxx::nothing) << "Level set to " #level << Log::endl; PrintLevels(log);
        LVL(logxx::debug);
        LVL(logxx::notice);
        LVL(logxx::warning);
        LVL(logxx::info);
        LVL(logxx::error);
        LVL(logxx::nothing);
#undef LVL
        Log::GlobalLogLevel(logxx::debug);
}

void TestLogging::PrintLevels(Log& log) const {
        Log currentLog(log, "PrintLevels");
        currentLog(logxx::debug)   << Log::endl;
        currentLog(logxx::notice)  << Log::endl;
        currentLog(logxx::warning) << Log::endl;
        currentLog(logxx::info)    << Log::endl;
        currentLog(logxx::error)   << Log::endl;
}

bool TestLogging::CompareFiles(const std::string& name) const {
        S_LOG("CompareFiles");
	log(logxx::debug) << "Comparing files {" << name << "}" << Log::endl;
        std::string output = dataDir + name + outputPrefix;
        std::string original = dataDir + name + etalonPrefix;
        std::ifstream res(output), etalon(original);
        if (res.good() && etalon.good()){
                while (res.good() && etalon.good()){
                        auto c1 = res.get();
                        auto c2 = etalon.get();
                        if (res.good() && !etalon.good()){
                                log(logxx::error) << "Problem while reading etalon file at " << res.tellg() << Log::endl;
                                return false;
                        } else if (!res.good() && etalon.good()){
                                log(logxx::error) << "Problem while reading result file at " << etalon.tellg() << Log::endl;
                                return false;
                        } else if (!res.good() || !etalon.good())
                                break;
                        if (c1 != c2){
                                log(logxx::error) << "At " << etalon.tellg() << ": [" << (char)c1 << "] != [" << (char)c2 << "]" << Log::endl;
                                return false;
                        }
                }
                return true;
        } else {
                log(logxx::error) << "Can't open files for comparison: {" << output << "} and {" << original << "}" <<
                        Log::endl;
                return false;
        }
}

bool TestLogging::RunTest() {
        S_LOG("RunTest");
        bool res;
        Log::GlobalLogLevel(logxx::debug);
        log(logxx::info) << "Running tests" << Log::endl;
        if (TestRedirection()){
                res = TestLevels();
        } else
                res = false;
        log.OverrideStream();
        if (res)
                log(logxx::info) << "OK" << Log::endl;
        else
                log(logxx::info) << "FAILED" << Log::endl;
        return res;
}

