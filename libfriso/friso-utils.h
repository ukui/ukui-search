#ifndef FRISOUTILS_H
#define FRISOUTILS_H

#include <string>
#include "libfriso-segmentation_global.h"

#ifdef __cplusplus
extern "C"{
#endif

#include "friso/src/friso_API.h"
#include "friso/src/friso.h"
#include "friso/src/friso_ctype.h"

#ifdef __cplusplus
}
#endif

#define FRISO_INI_PATH "/usr/share/ukui-search/res/friso.ini"

class LIBFRISOSEGMENTATION_EXPORT FrisoUtils
{
public:
    static int init();
    static int destroy();
    static void segementOnlyInSearch(::std::string&, char *);
    static friso_t g_friso;
    static friso_config_t g_config;
private:
    FrisoUtils() = delete;
};

#endif // FRISOUTILS_H
