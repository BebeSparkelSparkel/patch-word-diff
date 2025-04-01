#ifndef PP_UTILS_H
#define PP_UTILS_H

#define EMPTY_F(...)

#define COMMA ,
#define COMMA_F(...) COMMA

#define PIPE |
#define PIPE_F() PIPE

#define CAT_APP(x, y, ...) x y()

//#define IDENTITY(x) x

#define STRINGIFY_VAR_HELPER(x) #x
#define STRINGIFY_VAR(x) STRINGIFY_VAR_HELPER(x)

#endif
