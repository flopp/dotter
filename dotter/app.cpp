#include "app.h"
#include "config.h"

App::App(int& argc, char** argv) :
    QApplication(argc, argv),
    _config{new Config}
{
    setOrganizationName("flopp");
    setOrganizationDomain("flopp.net");
    setApplicationName("Dotter");

    _config->read();
}

App::~App()
{
    delete _config;
}

Config* App::config()
{
    return _config;
}

void App::quitting()
{
    _config->write();
}
