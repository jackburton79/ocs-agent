#include "http/URL.h"

#include <iostream>

const char* urls[] = {
    "http://www.test.com:90/test",
    "http://www.test2.it",
    "https://differentURL.com/part/of/path/end.xml",
    "www.withoutproto.com",
    "/pathonly/run.php",
    "user:pass@server/directory/file.xml",
    "http://user:password@server:81/directory/",
	"HTTP://user:password@server:81/directory/",
    NULL
};


int main()
{
    size_t i = 0;
    URL url;
    for (char* urlString = NULL; (urlString = (char*)urls[i]) != NULL; i++) {
        url.SetTo(urlString);
        std::cout << "url: " << url.URLString() << std::endl;
        std::cout << "\t" << "relative: " << (url.IsRelative() ? "yes" : "no") << std::endl;
        std::cout << "\t" << "protocol: " << url.Protocol() << std::endl;
        std::cout << "\t" << "host: " << url.Host() << std::endl;
        std::cout << "\t" << "port: " << url.Port() << std::endl;
        std::cout << "\t" << "path: " << url.Path() << std::endl;
        std::cout << "\t" << "username: " << url.Username() << std::endl;
        std::cout << "\t" << "password: " << url.Password() << std::endl;
    }
}
