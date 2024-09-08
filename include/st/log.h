#ifndef LOG_H_
#define LOG_H_

#if defined(ENABLE_LOG)
#define LOGN(str) std::cerr << str << std::endl
#define LOG(str) std::cerr << str;

#else
#define LOGN(str) {}
#define LOG(str) {}
#endif

#endif // LOG_H_
