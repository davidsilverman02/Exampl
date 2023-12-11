#define SDL_MAIN_HANDLED true
#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <SDL_image.h>
//#include <SDL_mixer.h>
//#include <SDL_ttf.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "Editor/ComponentPool.h"
#include "Editor/System.h"
#include <stdlib.h> 
#include <time.h> 
#include <cmath>
#include "Editor/StackAllocator.h"
#include<fstream>
#include<unordered_map>
#include "Editor/include/include/json/json.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef __EMSCRIPTEN__
EM_JS(int, canvas_get_width, (), {
return canvas.width;
});

EM_JS(int, canvas_get_height, (), {
return canvas.height;
});
#endif

GameObject gameObjects[MAX_GAMEOBJECTS];
ComponentPool<RectangleRenderer> renderers;
ComponentPool<RectangleCollider> colliders;
ComponentPool<PlayerController> controllers;
ComponentPool<ColliderColorChanger> changers;
ComponentPool<Transform> transforms;

ComponentPool<Component> comps;

vector<char*> cha;

typedef void (*compFn)(GameObject* go);

std::unordered_map<int, compFn> compCreationMap;

//IMGUI_API bool  InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
//IMGUI_API bool  InputTextMultiline(const char* label, std::string* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
//IMGUI_API bool  InputTextWithHint(const char* label, const char* hint, std::string* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

class X {
public:
    X(std::string& s) : s_{ s } { s.resize(len_); }
    ~X() { s_.resize(strlen(s_.c_str())); }
    operator char* () { return (char*)s_.data(); }
    static constexpr auto len() { return len_ - 1; }
private:
    std::string& s_;
    static constexpr auto len_ = 255;
};

int keyToInt(string key)
{
    if (key == "hasRectangle")
        return 0;
    if (key == "hasCollider")
        return 1;
    if (key == "hasPlayer")
        return 2;
    if (key == "hasColorChanger")
        return 3;
    if (key == "transform")
        return 4;
    else
        return -1;
}

const char* stringRet(string sti)
{
    return sti.c_str();
}

void addNothing(GameObject* go)
{

}

void makeRenderer(GameObject* go)
{
    go->renderer = renderers.New(go);
}

void makeCollider(GameObject* go)
{
    go->collider = colliders.New(go);
}

void makePlayerController(GameObject* go)
{
    go->player = controllers.New(go);
}

void makeColliderColorChanger(GameObject* go)
{
    go->colorChanger = changers.New(go);
}

void makeTransform(GameObject* go)
{
    go->transform = transforms.New(go);
}

void registerCompFns()
{
    compCreationMap[-1] = addNothing;
    compCreationMap[0] = makeRenderer;
    compCreationMap[1] = makeCollider;
    compCreationMap[2] = makePlayerController;
    compCreationMap[3] = makeColliderColorChanger;
    compCreationMap[4] = makeTransform;
}

int getTopIndex()
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (!gameObjects[i].isActive)
        {
            return i;
        }
    }
}

void loadLevel(string fileName)
{
    ifstream loadMe(fileName);
    Json::Value velu;
    Json::Value items;
    Json::Reader reader;

    int itemInd = 0;
    int compInd = -1;

    if (!reader.parse(loadMe, velu))
        std::cout << "failure" << endl;

    const Json::Value datasections = velu[""];

    items = datasections[0];

    for (auto const& id : velu.getMemberNames())
    {
        itemInd = getTopIndex();

        if (!velu[id].empty())
        {
            gameObjects[itemInd].isActive = true;
            string ih = id.c_str();
            gameObjects[itemInd].name = ih.c_str();
            for (auto const& ir : velu[id].getMemberNames())
            {
                if (ir == "transform")
                {
                    compCreationMap[keyToInt(ir)](&gameObjects[itemInd]);

                    if (!velu[id][ir]["x"].empty())
                    {
                        gameObjects[itemInd].transform->pos.x = velu[id][ir]["x"].asFloat();
                        //cout << velu[id][ir]["x"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["y"].empty())
                    {
                        gameObjects[itemInd].transform->pos.y = velu[id][ir]["y"].asFloat();
                        //cout << velu[id][ir]["y"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["z"].empty())
                    {
                        gameObjects[itemInd].transform->pos.y = velu[id][ir]["y"].asFloat();
                        //cout << velu[id][ir]["z"].asFloat() << endl;
                    }
                }
                else if (ir == "hasRectangle")
                {
                    compCreationMap[keyToInt(ir)](&gameObjects[itemInd]);

                    if (!velu[id][ir]["width"].empty())
                    {
                        gameObjects[itemInd].renderer->width = velu[id][ir]["width"].asFloat();
                        //cout << velu[id][ir]["width"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["height"].empty())
                    {
                        gameObjects[itemInd].renderer->height = velu[id][ir]["height"].asFloat();
                        //cout << velu[id][ir]["height"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["color"].empty())
                    {
                        if (!velu[id][ir]["color"]["r"].empty())
                        {
                            gameObjects[itemInd].renderer->color.x = gameObjects[itemInd].renderer->color.flooredVal(velu[id][ir]["color"]["r"].asFloat());
                            //cout << velu[id][ir]["color"]["r"].asFloat() << endl;
                            if (gameObjects[itemInd].colorChanger != nullptr)
                                gameObjects[itemInd].colorChanger->nonColColor.x = gameObjects[itemInd].renderer->color.x;
                        }

                        if (!velu[id][ir]["color"]["g"].empty())
                        {
                            gameObjects[itemInd].renderer->color.y = gameObjects[itemInd].renderer->color.flooredVal(velu[id][ir]["color"]["g"].asFloat());
                            //cout << velu[id][ir]["color"]["g"].asFloat() << endl;
                            if (gameObjects[itemInd].colorChanger != nullptr)
                                gameObjects[itemInd].colorChanger->nonColColor.y = gameObjects[itemInd].renderer->color.y;
                        }

                        if (!velu[id][ir]["color"]["b"].empty())
                        {
                            gameObjects[itemInd].renderer->color.z = gameObjects[itemInd].renderer->color.flooredVal(velu[id][ir]["color"]["b"].asFloat());
                            //cout << velu[id][ir]["color"]["b"].asFloat() << endl;
                            if (gameObjects[itemInd].colorChanger != nullptr)
                                gameObjects[itemInd].colorChanger->nonColColor.z = gameObjects[itemInd].renderer->color.z;
                        }
                    }
                }
                else if (ir == "hasCollider")
                {
                    if (velu[id][ir].asBool() == true)
                    {
                        compCreationMap[keyToInt(ir)](&gameObjects[itemInd]);
                    }
                    //cout << velu[id][ir].asBool() << endl;
                }
                else if (ir == "hasPlayer")
                {
                    compCreationMap[keyToInt(ir)](&gameObjects[itemInd]);
                    if (!velu[id][ir]["isPlayer"].empty())
                    {
                        gameObjects[itemInd].isPlayer = velu[id][ir]["isPlayer"].asBool();
                        //cout << velu[id][ir]["isPlayer"].asBool() << endl;
                    }
                }
                else if (ir == "hasColorChanger")
                {
                    compCreationMap[keyToInt(ir)](&gameObjects[itemInd]);
                    if (!velu[id][ir]["r"].empty())
                    {
                        gameObjects[itemInd].colorChanger->colColor.x = gameObjects[itemInd].colorChanger->colColor.flooredVal(velu[id][ir]["r"].asFloat());
                        //cout << velu[id][ir]["r"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["g"].empty())
                    {
                        gameObjects[itemInd].colorChanger->colColor.y = gameObjects[itemInd].colorChanger->colColor.flooredVal(velu[id][ir]["g"].asFloat());
                        //cout << velu[id][ir]["g"].asFloat() << endl;
                    }

                    if (!velu[id][ir]["b"].empty())
                    {
                        gameObjects[itemInd].colorChanger->colColor.z = gameObjects[itemInd].colorChanger->colColor.flooredVal(velu[id][ir]["b"].asFloat());
                        //cout << velu[id][ir]["b"].asFloat() << endl;
                    }
                    //gameObjects[itemInd].colorChanger->nonColColor.x = gameObjects[itemInd].renderer->color.x;
                    //gameObjects[itemInd].colorChanger->nonColColor.y = gameObjects[itemInd].renderer->color.y;
                    //gameObjects[itemInd].colorChanger->nonColColor.z = gameObjects[itemInd].renderer->color.z;
                }
            }
        }

        cout << endl;
    }
}

void WriteToLevel(string fileName)
{
    ofstream writeFile(fileName);
    Json::Value va;
    va["Obj1"]["transform"]["x"] = 9.0;
    string nom = "Obj";
    string num = "0";
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        num = to_string(i + 1);
        nom = "Obj" + num;
        if (gameObjects[i].isActive)
        {
            va[nom]["transform"]["x"] = gameObjects[i].transform->pos.x;
            va[nom]["transform"]["y"] = gameObjects[i].transform->pos.y;
            va[nom]["transform"]["z"] = gameObjects[i].transform->pos.z;


            if (gameObjects[i].renderer != NULL)
            {
                va[nom]["hasRectangle"]["width"] = gameObjects[i].renderer->width;
                va[nom]["hasRectangle"]["height"] = gameObjects[i].renderer->height;
                va[nom]["hasRectangle"]["color"]["r"] = gameObjects[i].renderer->color.x;
                va[nom]["hasRectangle"]["color"]["g"] = gameObjects[i].renderer->color.y;
                va[nom]["hasRectangle"]["color"]["b"] = gameObjects[i].renderer->color.z;
            }
            if (gameObjects[i].collider != NULL)
                va[nom]["hasCollider"] = true;
            if (gameObjects[i].player != NULL)
                va[nom]["hasPlayer"]["isPlayer"] = gameObjects[i].isPlayer;
            if (gameObjects[i].colorChanger != NULL)
            {
                va[nom]["hasColorChanger"]["r"] = gameObjects[i].colorChanger->colColor.x;
                va[nom]["hasColorChanger"]["g"] = gameObjects[i].colorChanger->colColor.y;
                va[nom]["hasColorChanger"]["b"] = gameObjects[i].colorChanger->colColor.y;
            }
        }
    }

    //cout << va << endl;
    writeFile << va << endl;
    writeFile.close();
}

void massMove(SDL_Event evn, bool isDown)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (gameObjects[i].isActive && gameObjects[i].player != NULL)
        {
            gameObjects[i].player->InputMovement(evn, isDown);
        }
    }
}

void localAllocate(int maxWid, int maxHei, bool totalAllocate)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (!gameObjects[i].isActive)
        {
            GameObject& obby = gameObjects[i];
            obby.isActive = true;
            // Heap allocations! Have to clean these up sometime :/
            obby.renderer = renderers.New(&obby);
            obby.collider = colliders.New(&obby);
            int rand1 = rand() % ((&gameObjects[0].transform->pos.x + maxWid) - (&gameObjects[0].transform->pos.x - maxWid)) + (gameObjects[0].transform->pos.x - maxWid);
            int rand2 = rand() % ((&gameObjects[0].transform->pos.x + maxHei) - (&gameObjects[0].transform->pos.x - maxHei)) + (gameObjects[0].transform->pos.x - maxHei);
            int rand3 = rand() % 256;
            int rand4 = rand() % 256;
            int rand5 = rand() % 256;
            obby.transform->pos.x = rand1;
            obby.transform->pos.y = rand2;
            obby.renderer->color.x = rand3;
            obby.renderer->color.y = rand4;
            obby.renderer->color.z = rand5;
            obby.renderer->height = 25;
            obby.renderer->width = 25;

            if (totalAllocate)
            {
                obby.player = controllers.New(&obby);
                obby.colorChanger = changers.New(&obby);
                obby.colorChanger->nonColColor.x = obby.renderer->color.x;
                obby.colorChanger->nonColColor.y = obby.renderer->color.y;
                obby.colorChanger->nonColColor.z = obby.renderer->color.z;
                obby.colorChanger->colColor.x = rand() % 256;
                obby.colorChanger->colColor.y = rand() % 256;
                obby.colorChanger->colColor.z = rand() % 256;
            }

            return;
        }
    }
};

void randAllocate(int maxWid, int maxHei, bool totalAllocate)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (!gameObjects[i].isActive && i < MAX_GAMEOBJECTS - 1)
        {
            GameObject& obby = gameObjects[i];
            obby.isActive = true;
            // Heap allocations! Have to clean these up sometime :/
            obby.renderer = renderers.New(&obby);
            obby.collider = colliders.New(&obby);
            obby.transform = transforms.New(&obby);
            int rand1 = rand() % maxWid;
            int rand2 = rand() % maxHei;
            int rand3 = rand() % 256;
            int rand4 = rand() % 256;
            int rand5 = rand() % 256;
            obby.transform->pos.x = rand1;
            obby.transform->pos.y = rand2;
            obby.renderer->color.x = rand3;
            obby.renderer->color.y = rand4;
            obby.renderer->color.z = rand5;
            obby.renderer->height = 25;
            obby.renderer->width = 25;

            if (totalAllocate)
            {
                obby.player = controllers.New(&obby);
                obby.colorChanger = changers.New(&obby);
                obby.colorChanger->nonColColor.x = obby.renderer->color.x;
                obby.colorChanger->nonColColor.y = obby.renderer->color.y;
                obby.colorChanger->nonColColor.z = obby.renderer->color.z;
                obby.colorChanger->colColor.x = rand() % 256;
                obby.colorChanger->colColor.y = rand() % 256;
                obby.colorChanger->colColor.z = rand() % 256;
            }

            return;
        }
    }
};

void allocateAll(int maxWid, int maxHei)
{
    srand(time(NULL));

    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (!gameObjects[i].isActive)
        {
            GameObject& obby = gameObjects[i];
            obby.isActive = true;
            if (obby.transform == NULL)
            {
                obby.transform = transforms.New(&obby);
                obby.transform->pos.x = rand() % maxWid;
                obby.transform->pos.y = rand() % maxHei;
            }


            if (obby.collider == NULL)
            {
                obby.collider = colliders.New(&obby);
            }

            if (obby.renderer == NULL)
            {
                obby.renderer = renderers.New(&obby);
                obby.renderer->color.x = rand() % 256;
                obby.renderer->color.y = rand() % 256;
                obby.renderer->color.z = rand() % 256;
                obby.renderer->height = 50;
                obby.renderer->width = 50;
            }

            if (obby.player == NULL)
            {
                obby.player = controllers.New(&obby);
            }

            if (obby.colorChanger == NULL)
            {
                obby.colorChanger = changers.New(&obby);
                obby.colorChanger->nonColColor.x = obby.renderer->color.x;
                obby.colorChanger->nonColColor.y = obby.renderer->color.y;
                obby.colorChanger->nonColColor.z = obby.renderer->color.z;
                obby.colorChanger->colColor.x = rand() % 256;
                obby.colorChanger->colColor.y = rand() % 256;
                obby.colorChanger->colColor.z = rand() % 256;
            }
        }
        else
        {
            if (gameObjects[i].collider == NULL)
            {
                gameObjects[i].collider = colliders.New(&gameObjects[i]);
            }

            if (gameObjects[i].renderer == NULL)
            {
                gameObjects[i].renderer = renderers.New(&gameObjects[i]);
                gameObjects[i].renderer->color.x = rand() % 256;
                gameObjects[i].renderer->color.y = rand() % 256;
                gameObjects[i].renderer->color.z = rand() % 256;
                gameObjects[i].renderer->height = 50;
                gameObjects[i].renderer->width = 50;
            }

            if (gameObjects[i].player == NULL)
            {
                gameObjects[i].player = controllers.New(&gameObjects[i]);
            }

            if (gameObjects[i].colorChanger == NULL)
            {
                gameObjects[i].colorChanger = changers.New(&gameObjects[i]);
                gameObjects[i].colorChanger->nonColColor.x = gameObjects[i].renderer->color.x;
                gameObjects[i].colorChanger->nonColColor.y = gameObjects[i].renderer->color.y;
                gameObjects[i].colorChanger->nonColColor.z = gameObjects[i].renderer->color.z;
                gameObjects[i].colorChanger->colColor.x = rand() % 256;
                gameObjects[i].colorChanger->colColor.y = rand() % 256;
                gameObjects[i].colorChanger->colColor.z = rand() % 256;
            }
        }
    }
}

// num = 0 transform, num = 1 renderer, num = 2 collider, num = 3 player, num = 4 color changer, 
void allocateGivenNum(int num, int index)
{
    switch (num)
    {
    case 0:
        if (gameObjects[index].transform == NULL)
        {
            gameObjects[index].transform = transforms.New(&gameObjects[index]);
        }
        break;
    case 1:
        if (gameObjects[index].renderer == NULL)
        {
            gameObjects[index].renderer = renderers.New(&gameObjects[index]);
        }
        break;
    case 2:
        if (gameObjects[index].collider == NULL)
        {
            gameObjects[index].collider = colliders.New(&gameObjects[index]);
        }
        break;
    case 3:
        if (gameObjects[index].player == NULL)
        {
            gameObjects[index].player = controllers.New(&gameObjects[index]);
        }
        break;
    case 4:
        if (gameObjects[index].colorChanger == NULL)
        {
            gameObjects[index].colorChanger = changers.New(&gameObjects[index]);
        }
        break;
    default:
        break;
    }
}

// num = 0 transform, num = 1 renderer, num = 2 collider, num = 3 player, num = 4 color changer,  
void deallocateGivenNum(int num, int index)
{
    switch (num)
    {
    case 0:
        gameObjects[index].transform = NULL;
        transforms.Delete(&gameObjects[index]);
        break;
    case 1:
        gameObjects[index].renderer = NULL;
        renderers.Delete(&gameObjects[index]);
        break;
    case 2:
        gameObjects[index].collider = NULL;
        colliders.Delete(&gameObjects[index]);
        break;
    case 3:
        gameObjects[index].player = NULL;
        controllers.Delete(&gameObjects[index]);
        break;
    case 4:
        gameObjects[index].colorChanger = NULL;
        changers.Delete(&gameObjects[index]);
        break;
    default:
        break;
    }
}

void deallocate(GameObject* deleter)
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (&gameObjects[i] == deleter)
        {
            gameObjects[i].renderer = NULL;
            renderers.Delete(deleter);
            gameObjects[i].collider = NULL;
            colliders.Delete(deleter);
            gameObjects[i].player = NULL;
            controllers.Delete(deleter);
            gameObjects[i].colorChanger = NULL;
            changers.Delete(deleter);
            gameObjects[i].transform = NULL;
            transforms.Delete(deleter);
            gameObjects[i].isActive = false;

        }
    }
}

void deallocateClosest()
{
    int toDA = -1;

    float inf = std::numeric_limits<float>::infinity();

    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (!gameObjects[i].isPlayer && gameObjects[i].isActive)
        {
            if (sqrt(pow(gameObjects[0].transform->pos.x - gameObjects[i].transform->pos.x, 2) + pow(gameObjects[0].transform->pos.y - gameObjects[i].transform->pos.y, 2)) < inf)
            {
                inf = sqrt(pow(gameObjects[0].transform->pos.x - gameObjects[i].transform->pos.x, 2) + pow(gameObjects[0].transform->pos.y - gameObjects[i].transform->pos.y, 2));
                toDA = i;
            }
        }
    }

    if (toDA != -1)
    {
        deallocate(&gameObjects[toDA]);
    }
}

void deallocateAll()
{
    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (gameObjects[i].isPlayer == false)
        {
            deallocate(&gameObjects[i]);
        }
    }
}

void rapidSuccession(int maxWid, int maxHei, bool totalAllocate)
{
    int randAct = rand() % 3 + 1;
    int randDel = rand() % 3 + 1;

    int hasGone = 0;
    int forther = 0;

    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (gameObjects[i].isPlayer == false && gameObjects[i].isActive)
        {
            deallocate(&gameObjects[i]);
            hasGone++;
            if (hasGone >= randDel)
                i = MAX_GAMEOBJECTS;
        }
    }

    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (gameObjects[i].isPlayer == false && gameObjects[i].isActive == false)
        {
            randAllocate(maxWid, maxHei, totalAllocate);
            forther++;
            if (forther >= randAct)
                i = MAX_GAMEOBJECTS;
        }
    }
}

bool allocating;
StackAllocator ColliderAlloc(65536);
StackAllocator MultsOfFive(65536);

void AllocateColliders(Vector3 poso)
{
    RectangleCollider* colls = ColliderAlloc.New<RectangleCollider>();
    size_t arrayLen = 0;

    for (int i = 0; i < MAX_GAMEOBJECTS; i++)
    {
        if (colliders.isActive[i])
        {
            if (colliders.components[i].isInside(poso))
            {
                RectangleCollider* arraySlot = ColliderAlloc.PushArray<RectangleCollider>();
                *arraySlot = colliders.components[i];
                //arraySlot->obj = colliders.components[i].obj;
                colls[arrayLen].obj = colliders.components[i].obj;
                arrayLen++;

            }
        }
    }

    if (allocating)
    {
        std::cout << "Inside colliders: " << arrayLen << "\n\n";
        for (int i = 0; i < arrayLen; i++)
        {
            std::cout << colls[i].obj->transform->pos.x << "   " << colls[i].obj->transform->pos.y << std::endl;
        }
    }
}

void AllocateFives()
{
    int* fives = MultsOfFive.New<int>();
    size_t arrayLen = 0;
    srand(time(NULL));

    for (int i = 0; i < 15; i++)
    {
        int dieRoll = rand() % 100;
        if (dieRoll % 5 == 0)
        {
            int* arraySlot = MultsOfFive.PushArray<int>();
            *arraySlot = dieRoll;
            arrayLen++;
        }
    }

    if (allocating)
    {
        std::cout << "Integers that are multiples of 5:\n\n";
        for (int i = 0; i < arrayLen; i++)
        {
            std::cout << fives[i] << std::endl;
        }
    }
}

void DeallocateAllocators()
{
    ColliderAlloc.Clear();
    MultsOfFive.Clear();
    //system("clr");
    allocating = false;
}

void writetoLevels(string lvlName)
{

}

int main(int argc, char* argv[]) {
    // Unused argc, argv
    (void) argc;
    (void) argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    auto width = 1280;
    auto height = 720;
#ifdef __EMSCRIPTEN__
    width = canvas_get_width();
    height = canvas_get_height();
#endif

    // Setup window
    SDL_WindowFlags window_flags = (SDL_WindowFlags) (SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    SDL_Window *window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, width, height, window_flags);

    if (!window) {
        std::cout << "Window could not be created!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        abort();
    }

    // Setup SDL_Renderer instance
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        SDL_Log("Error creating SDL_Renderer!");
        abort();
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;

    // Declare rect of square
    SDL_Rect squareRect;

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = std::min(width, height) / 2;
    squareRect.h = std::min(width, height) / 2;

    // Square position: In the middle of the screen
    squareRect.x = width / 2 - squareRect.w / 2;
    squareRect.y = height / 2 - squareRect.h / 2;

    // Event loop
    while (!done) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
           
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
            if(event.type == SDL_KEYDOWN)
            massMove(event, true);
            if(event.type == SDL_KEYUP)
            massMove(event, false);
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }


        // Rendering
        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);

        // todo: add your game logic here to be drawn before the ui rendering
        // Set renderer color red to draw the square
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        // Draw filled square
        SDL_RenderFillRect(renderer, &squareRect);

        // present ui on top of your drawings
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

        SDL_Delay(0);
    }

    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}