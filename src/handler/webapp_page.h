#ifndef WEBAPP_PAGE_H_INCLUDED
#define WEBAPP_PAGE_H_INCLUDED

#include <string>

#include "server/webserver.h"

namespace webapp_page {

std::string page(Request &, Response &response);

} // namespace webapp_page

#endif // WEBAPP_PAGE_H_INCLUDED
