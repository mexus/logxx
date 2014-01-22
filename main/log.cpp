#include <string>
#include <memory>
#include <fstream>

#include "log.h"

namespace logxx {

        std::mutex Log::lock;
        const std::map<Log::LogLevel, std::string> Log::levelMessages {
                {Log::debug,    " Debug "},
                {Log::notice,   "Notice "},
                {Log::warning,  "Warning"},
                {Log::info,     " Info  "},
                {Log::error,    " Error "},
                {Log::nothing,  "Nothing"}
        };
        std::ostream Log::cnull(0);
        std::ostream* Log::defaultStream(&std::cout);
        Log::LogLevel Log::globalLogLevel = Log::debug;

        Log::Log(const std::string& label) : label(label) {

        }


        Log::Log(const Log& old, const std::string& label, bool inheritLevel) : label(old.label) {
                if (old.overrideLogLevel){
                        if (inheritLevel)
                                overrideLogLevel = old.overrideLogLevel;
                        else
                                OverrideLogLevel(*(old.overrideLogLevel.get()));
                }
                if (old.overrideStream)
                        OverrideStream(old.overrideStream);
                AddLabel(label);
        }

        Log::~Log() {
        }

        void Log::AddLabel(const std::string& newLabel) {
                if (!newLabel.empty()) {
                        if (!label.empty())
                                label += "::";
                        label += newLabel;
                }
        }

        std::ostream& Log::endl(std::ostream& s) {
                s << std::endl;
                lock.unlock();
                return s;
        }

        std::ostream& Log::Print(std::ostream& s) const {
                return s;
        }

        void Log::SetLogLevel(Log::LogLevel l) {
                globalLogLevel = l;
        }

        Log::LogLevel Log::GetLogLevel() const {
                if (overrideLogLevel)
                        return *(overrideLogLevel.get());
                else
                        return globalLogLevel;
        }

        void Log::OverrideLogLevel(){
                overrideLogLevel.reset();
        }

        void Log::OverrideLogLevel(Log::LogLevel l) {
                if (overrideLogLevel){
                        *(overrideLogLevel.get()) = l;
                } else
                        overrideLogLevel = std::make_shared<LogLevel>(l);
        }

        std::ostream& Log::GetStream() {
                if (overrideStream)
                        return *(overrideStream.get());
                else
                        return *defaultStream;
        }

        void Log::OverrideStream() {
                overrideStream.reset();
        }

        void Log::OverrideStream(const std::shared_ptr<std::ostream>& stream) {
                overrideStream = stream;
        }

        void Log::OverrideStream(const std::string& fileName){
                OverrideStream(std::shared_ptr<std::ostream>(new std::ofstream(fileName)));
        }

        void Log::SetStream(std::ostream& stream) {
                defaultStream = &stream;
        }

} //namespace logxx
