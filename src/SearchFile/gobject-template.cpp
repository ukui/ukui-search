#include "gobject-template.h"

std::shared_ptr<gobjecttemplate<GFile>> wrapGFile(GFile *file) {
    return std::make_shared<gobjecttemplate<GFile>>(file);
}

std::shared_ptr<gobjecttemplate<GFileInfo>> wrapGFileInfo(GFileInfo *info) {
    return std::make_shared<gobjecttemplate<GFileInfo>>(info);
}
