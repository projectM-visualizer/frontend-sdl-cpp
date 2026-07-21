#include "ProjectMSDLApplication.h"

#ifdef USE_SDL3
# include <SDL3/SDL.h>
#else
# include <SDL2/SDL.h>
#endif

int main(int argc, char* argv[])
{
    Poco::AutoPtr<ProjectMSDLApplication> pApp = new ProjectMSDLApplication;
    try
    {
        pApp->init(argc, argv);
    }
    catch (Poco::Exception& exc)
    {
        pApp->logger().log(exc);
        return Poco::Util::Application::EXIT_CONFIG;
    }
    return pApp->run();
}
