#ifndef COMMONDEFINES_H
#define COMMONDEFINES_H
#include <QFlags>
namespace UkuiSearch {
/**
 * @brief The SearchType enum
 *
 */
enum class SearchType
{
    File        = 1u << 0,
    FileContent = 1u << 1,
    Application = 1u << 2,
    Setting     = 1u << 3,
    Note        = 1u << 4,
    Mail        = 1u << 5,
    Custom      = 1u << 6
};

/**
 * @brief The ResultDataType enum
 *
 */
enum ResultDataType
{
    FilePath                     = 1u << 0,
    FileIconName                 = 1u << 1,
    FileName                     = 1u << 2,
    ModifiedTime                 = 1u << 3,
    ApplicationDesktopPath       = 1u << 4,
    ApplicationLocalName         = 1u << 5,
    ApplicationIconName          = 1u << 6,
    ApplicationDescription       = 1u << 7,
    IsOnlineApplication          = 1u << 8
    //add more...

};
Q_DECLARE_FLAGS(ResultDataTypes, ResultDataType)

}

Q_DECLARE_OPERATORS_FOR_FLAGS(UkuiSearch::ResultDataTypes)

#endif // COMMONDEFINES_H
