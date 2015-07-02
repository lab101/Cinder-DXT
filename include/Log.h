#ifndef POLY_LOG_H
#define POLY_LOG_H

#include <string>
#include <fstream>
#include <iostream>
#define SX_LOG_LEVEL_ALL 5
#define SX_LOG_LEVEL_ERROR  1
#define SX_LOG_LEVEL_WARNING 2
#define SX_LOG_LEVEL_VERBOSE 3
#define SX_LOG_LEVEL_DEBUG 4

#if defined(_MSC_VER)
#  define SX_DEBUG(fmt, ...) { std::cout<< fmt<<std::endl;} 
#  define SX_VERBOSE(fmt, ...) { std::cout<< fmt<<std::endl;} 
#  define SX_WARNING(fmt, ...) {std::cout<< fmt<<std::endl; } 
#  define SX_ERROR(fmt, ...) { std::cout<< fmt<<std::endl; } 
#else                                                                             
#  define SX_DEBUG(fmt, ...) { poly_debug(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define SX_VERBOSE(fmt, ...) { poly_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define SX_WARNING(fmt, ...) { poly_warning(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define SX_ERROR(fmt, ...) { poly_error(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#endif

/* --------------------------------------------------------------------------------- */

namespace poly {

  /* Debug control */
  int poly_log_init(std::string path = "");
  void poly_log_disable_stdout();
  void poly_log_enable_stdout();
  void poly_log_disable_log_to_file();
  void poly_log_enable_log_to_file();
  void poly_log_set_level(int level);
  int poly_log_get_level();

  /* Debug wrappers. */
  void poly_debug(int line, const char* function, const char* fmt, ...);
  void poly_verbose(int line, const char* function, const char* fmt, ...);
  void poly_warning(int line, const char* function, const char* fmt, ...);
  void poly_error(int line, const char* function, const char* fmt, ...);

  /* --------------------------------------------------------------------------------- */

  class Log {
  public:
    Log();
    ~Log();
    int open(std::string filepath);        /* Open the log with the give filepath. */
    void log(int level,                    /* Log something at the given level. */
             int line,                     /* Refers to the line in the source code. */
             const char* function,         /* Funcion that logged the message. */
             const char* fmt,              /* We use printf() like formats. */ 
             va_list args);                /* Variable arguments. */ 

  public:
    bool write_to_stdout;                  /* Write output also to stdout. */
    bool write_to_file;                    /* Write log to file. */
    int level;                             /* What level we should log. */
                                           
  private:                                 
    std::string filepath;                  /* Filepath where we save the log file. */
    std::ofstream ofs;                     /* The output file stream */
  };

  /* --------------------------------------------------------------------------------- */

  extern Log poly_log;

} /* namespace poly */

#endif
