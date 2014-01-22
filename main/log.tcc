#include <iostream>
#include "log.h"

namespace logxx {

        template<class ...Args>
        std::ostream& Log::operator()(Log::LogLevel level, Args... args){
                if (level >= GetLogLevel()){
                        lock.lock();
                        auto &stream = GetStream();
                        auto &levelMessage = levelMessages.at(level);
                        if (label.empty())
                                return Print(stream, levelMessage, args...);
                        else
                                return Print(stream, levelMessage, label, args...);
                } else
                        return cnull;
        }

        template<class T, class ...Args>
        std::ostream& Log::Print(std::ostream& s, T t, Args... args) const{
                s << "[" << t << "] ";
                return Print(s, args...);
        }
        
} // namespace logxx
