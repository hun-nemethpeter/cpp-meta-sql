#ifndef __SRC_TRACE_DEBUG_H__
#define __SRC_TRACE_DEBUG_H__

const char* IndentString(int p_level);

#include <sstream>
#define DEBUG(msg) \
        do { \
            std::ostringstream ___ostr___; \
                ___ostr___ << msg; \
            std::cout << __FILE__ << ":" << __LINE__ << " " << ___ostr___.str() << std::endl; \
        } while (0)
#endif
