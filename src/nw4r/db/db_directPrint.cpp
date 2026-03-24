#include <nw4r/db.h>


namespace nw4r {
namespace db {

void DirectPrint_Printf(int posh, int posv, const char *format, ...) {
    va_list list;
#ifdef DEBUG
    va_start(list, format);
    detail::DirectPrint_DrawStringToXfb(posh, posv, format, &list, true, true);
    va_end(list);
#endif
}

} // namespace db
} // namespace nw4r