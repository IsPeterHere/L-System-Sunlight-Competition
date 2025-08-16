#include "BaseApp.h"
#include "..\L-System-Sunlight-Competition\LSSC.h"

class INSTANCE
{
public:
    static INSTANCE* get_INSTANCE()
    {
        static bool first_call{ true };
        if (first_call)
            instance = new INSTANCE();
        first_call = false;
        return instance;
    }
    void main()
    {
        app.vertices =
        {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };

        app.indices =
        {
        0, 1, 2
        };
        app.flush_mesh_update();

        instance = this;

        std::cout << "LSSC" << "\n";
        std::cout << "Press P to pause/unpause" << "\n";
        std::cout << "HOLD G to print day summary" << "\n";
        std::cout << "Press T to switch between seeing day or skipping day" << "\n";
        std::cout << "\n" << "\n";
        world.start();
        app.run_with_update_function(&update, 0.5);
        world.end();
    }

    static bool update()
    {
        instance->display.update_screen_dimensions(instance->app.getWindowExtent().width, instance->app.getWindowExtent().height);
        if (!instance->app.control->p)
        {
            if (instance->app.control->t)
                instance->world.segment_day();
            else
                instance->world.day(instance->app.control->g);

            if (instance->world.get_number_of_species() == 0)
            {
                std::cout << "\n" << "\n" << "No Species Remain" << "\n" << "\n";
                instance->app.close_window();
            }
            return true;
        }
        return false;
    }

    BaseApp app{ 800, 600 };
    Display display{ &app.vertices,&app.indices };
    World world{ &display };


private:
    static INSTANCE* instance;

    INSTANCE() {}
};

INSTANCE* INSTANCE::instance;

int main() 
{
    try 
    {
        INSTANCE* inst = INSTANCE::get_INSTANCE();
        inst->main();
        delete inst;
    }
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}