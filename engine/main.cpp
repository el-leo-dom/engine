#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_image/SDL_image.h"
#include <string>



class LTexture {
public:

    //inicia as variaveis
    LTexture();

    //dá cleanup as variaveis
    ~LTexture();

    //dá load a um ficheiro dado path
    bool loadFromFile(std::string path);

    //dá cleanup na textura
    void destroy();

    //desenha a textura
    void render(float x, float y);

    //dá get nas dimensoes da textura
    int getWidth();
    int getHeight();

private:

    //contem texture data (nao sei o que é ainda)
    SDL_Texture* mTexture;

    //dimensoes da textura
    int mWidth;
    int mHeight;
};

constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };

SDL_Window* gWindow{ nullptr };

SDL_Renderer* gRenderer{ nullptr };



LTexture gPngTexture1;
LTexture gPngTexture2;


LTexture::LTexture() :
    //inicializar as variaveis da textura
    mTexture{ nullptr },
    mWidth{ 0 },
    mHeight{ 0 }
{

};

LTexture::~LTexture()
{
    //clean up
    destroy();
};

bool LTexture::loadFromFile(std::string path) {

    //limpar antes de criar caso já exista algo
    destroy();

    //dar load na superficie
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == nullptr) {

        SDL_Log("Nao deu para dar load na imagem %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());

    } else {

        mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);

        if (mTexture == nullptr) {

            SDL_Log("Nao deu para criar a textura %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());

        } else {
            //ir buscar as dimensoes
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        //dar clean up na superficie
        SDL_DestroySurface(loadedSurface);
    }

    //return em sucesso se a textura deu load
    return mTexture != nullptr;

}

void LTexture::destroy() {
    // dar clean up na textura
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void LTexture::render(float x, float y) {
    // meter a textura na posição
    SDL_FRect dstRect = { x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

    //render na textura
    SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
}

int LTexture::getWidth() {

    return mWidth;
}

int LTexture::getHeight() {

    return mHeight;
}




bool init()
{
    //A flag de inicialização
    bool success{ true };

    //Iniciar o SDL

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("O SDL não deu para inicializar! Erro de SDL: %s\n", SDL_GetError());
        success = false;
    }
    else {
        // criar a janela

        if (!SDL_CreateWindowAndRenderer("SDL3 Tutorial : BOAS", kScreenWidth, kScreenHeight, 0, &gWindow, &gRenderer)) {
            SDL_Log("Janela falhou a ser criada! SDL error: %s\n", SDL_GetError());
            success = false;
        }
    }
    return success;
};

bool loadMedia() {

    // flag de sucesso
    bool success{ true };
    
    // load na splash image
    success = gPngTexture1.loadFromFile("ron.png");
    success = gPngTexture2.loadFromFile("snail.bmp");
    if (!success) {
        SDL_Log("Não deu para dar load no png");
    };

    return success;
};

void close() {
    
    //clean up na textura

    gPngTexture1.destroy();
    gPngTexture2.destroy();

    //destruir a janela e o renderer
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    //desligar
    SDL_Quit();
};





int main(int argc, char** argv)
{
    //exit code final
    int exitCode{ 0 };

    //inicializar

    if (!init()) {

        SDL_Log("Não conseguiu inicializar!\n");
        exitCode = 1;

    }
    else {

        //dar load
        if (!loadMedia()) {
            SDL_Log("Não deu load na media\n");
            exitCode = 2;
        }
        else {
            
            //flag de dar quit
            bool quit{ false };

            //event data (ainda nao sei o que isto é)
            SDL_Event e;
            SDL_zero( e );

            //main loop agora

            while ( quit == false ) {
                
                while (SDL_PollEvent(&e)) {

                    //testar se o event é do tipo quit

                    if (e.type == SDL_EVENT_QUIT) {

                        //fechar o main loop
                        quit = true;
                    }
                }

                //pintar de branco a superficie
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //render na imagem
                gPngTexture1.render(0.f, 0.f);
                gPngTexture2.render(200.f, 200.f);

                //update no screen
                SDL_RenderPresent(gRenderer);
            }

            //limpar tudo e desligar
            close();

            return exitCode;
        }
    }

    return 0;
}