#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>



class LTexture {
public:

    //constante simbolica de stretch acho eu
    
    static constexpr float kOriginalSize = -1.f;

    //inicia as variaveis
    LTexture();

    //dá cleanup as variaveis
    ~LTexture();

    //dá load a um ficheiro dado path
    bool loadFromFile(std::string path);

    //Creates texture from text
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

    //dá cleanup na textura
    void destroy();

    // cor
    void setColor(Uint8 r, Uint8 g, Uint8 b);

    // opacidade
    void setAlpha(Uint8 alpha);

    // blend mode (nao sei o que é ainda)
    void setBlending( SDL_BlendMode blendMode);

    //desenha a textura
    void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize, double degrees = 0.0, SDL_FPoint* center = nullptr);

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



LTexture gUpTexture, gDownTexture;


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
    
    success &= gUpTexture.loadFromFile("ron.png");
    if (!success) {
        SDL_Log("Não deu para dar load no ron png");
    };
    success &= gDownTexture.loadFromFile("snail.bmp");
    if (!success) {
        SDL_Log("Não deu para dar load no snail bmp");
    };

    return success;
};

void close() {
    
    //clean up na textura

    gUpTexture.destroy();
    gDownTexture.destroy();

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

            LTexture* currentTexture = &gUpTexture;

            SDL_Color bgColor = { 0xFF, 0xFF, 0xFF, 0xFF };

            //main loop agora

            while ( quit == false ) {
                
                while (SDL_PollEvent(&e)) {

                    //testar se o event é do tipo quit

                    if (e.type == SDL_EVENT_QUIT) {

                        //fechar o main loop
                        quit = true;
                    }

                    // isto verifica o cima ou baixo
                    else if (e.type == SDL_EVENT_KEY_DOWN) {
                        if (e.key.key == SDLK_UP) {
                            currentTexture = &gDownTexture;
                        }
                        else if (e.key.key == SDLK_DOWN) {
                            currentTexture = &gUpTexture;
                        }
                    }
                }

                // reset da cor para branco
                bgColor.r = 0xFF;
                bgColor.g = 0xFF;
                bgColor.b = 0xFF;


                // isto vai buscar o pointer para um array que tem um 1 ou 0 para cada pressed key atualmente

                const bool* keyStates = SDL_GetKeyboardState(nullptr);

                if (keyStates[SDL_SCANCODE_UP]) {
                    //RED
                    bgColor.r = 0xFF;
                    bgColor.g = 0x00;
                    bgColor.b = 0x00;
                }
                else if (keyStates[SDL_SCANCODE_DOWN]) {
                    //GREEN
                    bgColor.r = 0x00;
                    bgColor.g = 0xFF;
                    bgColor.b = 0x00;
                }


                //pintar de branco a superficie
                SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
                SDL_RenderClear(gRenderer);

                //render na imagem
                currentTexture->render(( kScreenWidth - currentTexture->getWidth()) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f);

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