#ifndef COMMONDEFINES_H
#define COMMONDEFINES_H
namespace UkuiSearch {
enum class SearchType
{
    File        = 0x1 << 0,
    FileContent = 0x1 << 1,
    Application = 0x1 << 2,
    Setting     = 0x1 << 3,
    Note        = 0x1 << 4,
    Mail        = 0x1 << 5,
    Custom      = 0x1 << 6
};
}
#endif // COMMONDEFINES_H
