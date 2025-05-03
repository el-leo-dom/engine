#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <string>
#include <sstream>



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
    void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize, double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipMode = SDL_FLIP_NONE);

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


class LButton {
public:
    //dimensoes
    static constexpr int kButtonWidth = 300;
    static constexpr int kButtonHeight = 200;

    LButton();

    //posicao do top left
    void setPosition(float x, float y);

    //events do rato

    void handleEvent(SDL_Event* e);

    void render();

    SDL_FPoint getPosition() const {
        return mPosition;
    }

    SDL_FPoint getVelocity() const {
        return mVelocity;
    }

    void setVelocity(float x, float y) {
        mVelocity.x = x;
        mVelocity.y = y;
    }

private:
    enum eButtonSprite {
        eButtonSpriteMouseOut = 0,
        eButtonSpriteMouseOverMotion = 1,
        eButtonSpriteMouseDown = 2,
        eButtonSpriteMouseUp = 3
    };

    //top left posicao
    SDL_FPoint mPosition;

    SDL_FPoint mVelocity;

    //sprite em uso atualmente

    eButtonSprite mCurrentSprite;
};


class LTimer {
public:
    LTimer();

    void start();
    void stop();
    void pause();
    void unpause();

    Uint64 getTicksNS();

    bool isStarted();
    bool isPaused();

private:

    Uint64 mStartTicks;

    Uint64 mPausedTicks;

    bool mPaused;
    bool mStarted;

};




constexpr int kScreenWidth{ 640 };
constexpr int kScreenHeight{ 480 };
constexpr int kScreenFPS{ 60 };
constexpr float accDueToGravity{ 0.5f };

SDL_Window* gWindow{ nullptr };

SDL_Renderer* gRenderer{ nullptr };

TTF_Font* gFont{ nullptr };

LTexture gUpTexture, gDownTexture, gTextTexture, gButtonSpriteTexture, gDotTexture;


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

void LTexture::render(float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipMode) {
    // meter a textura na posição
    SDL_FRect dstRect = { x, y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

    //logica de clippar
    if (clip != nullptr) {
        dstRect.w = clip->w;
        dstRect.h = clip->h;
    }

    //logica de resize 
    if (width > 0) {
        dstRect.w = width;
    }
    if (height > 0) {
        dstRect.h = height;
    }


    //render na textura
    SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode);
}

int LTexture::getWidth() {

    return mWidth;
}

int LTexture::getHeight() {

    return mHeight;
}


bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
    
    destroy();

    SDL_Surface* textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), 0, textColor);
    if (textSurface == nullptr) {
        SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
    }
    else {
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if (mTexture == nullptr) {
            SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
        }
        else {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_DestroySurface(textSurface);
    }

    return mTexture != nullptr;

}

LButton::LButton():
    mPosition{ 0.f, 0.f},
    mCurrentSprite{ eButtonSpriteMouseOut }
{

}


void LButton::setPosition(float x, float y) {
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e) {
    //verifivar se mouse event aconteceu

    if (e->type == SDL_EVENT_MOUSE_MOTION || e->type == SDL_EVENT_MOUSE_BUTTON_DOWN || e->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        // pegar na posicao do rato
        float x = -1.f, y = -1.f;

        SDL_GetMouseState(&x, &y);

        bool inside = true;

        if (x < mPosition.x) {
            inside = false;
        }
        else if (x > mPosition.x + kButtonWidth) {
            inside = false;
        }
        else if (y < mPosition.y) {
            inside = false;
        }
        else if (y > mPosition.y + kButtonHeight) {
            inside = false;
        }

        if (!inside) {
            mCurrentSprite = eButtonSpriteMouseOut;
        }
        else {
            switch (e->type) {
            case SDL_EVENT_MOUSE_MOTION:
                mCurrentSprite = eButtonSpriteMouseOverMotion;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                mCurrentSprite = eButtonSpriteMouseDown;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                mCurrentSprite = eButtonSpriteMouseUp;
                break;
            }
        }
    }
}

void LButton::render() {
    // definir os sprites clipados da imagem
    SDL_FRect spriteClips[] = {
        { 0.f, 0 * kButtonHeight, kButtonWidth, kButtonHeight },
        { 0.f, 1 * kButtonHeight, kButtonWidth, kButtonHeight },
        { 0.f, 2 * kButtonHeight, kButtonWidth, kButtonHeight },
        { 0.f, 3 * kButtonHeight, kButtonWidth, kButtonHeight },
    };

    //mostrar o atual

    gButtonSpriteTexture.render(mPosition.x, mPosition.y, &spriteClips[mCurrentSprite]);
}

LTimer::LTimer():
mStartTicks{ 0 },
mPausedTicks{ 0 },
mPaused{ false }, 
mStarted{ false }
{

}

void LTimer::start() {
    mStarted = true;
    mPaused = false;
    mStartTicks = SDL_GetTicksNS();
    mPausedTicks = 0;
}

void LTimer::stop() {
    mStarted = false;
    mPaused = false;
    mStartTicks = 0;
    mPausedTicks = 0;
}


void LTimer::pause() {
    if (mStarted && !mPaused) {
        mPaused = true;

        mPausedTicks = SDL_GetTicksNS() - mStartTicks;
        mStartTicks = 0;
    }
}

void LTimer::unpause() {
    if (mStarted && mPaused) {
        mPaused = false;

        mStartTicks = SDL_GetTicksNS() - mPausedTicks;
        mPausedTicks = 0;
    }
}

Uint64 LTimer::getTicksNS() {
    Uint64 time = 0;

    if (mStarted) {
        if (mPaused) {
            time = mPausedTicks;
        }
        else {
            time = SDL_GetTicksNS() - mStartTicks;
        }
    }
    return time;
}

class Dot {
public:

    static constexpr int kDotWidth = 20;
    static constexpr int kDotHeight = 20;

    static constexpr float kDotVel = 5.0f;

    Dot(); 

    void handleEvent(SDL_Event& e);

    void move();

    void render();

private:

    float mPosX, mPosY;
    float mVelX, mVelY;
};


Dot::Dot():
    mPosX{ 0 },
    mPosY{ 0 },
    mVelX{ 0 },
    mVelY{ 0 }
{

}

void Dot::handleEvent(SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN && e.key.repeat == 0) {
        switch (e.key.key) {
        case SDLK_UP: mVelY -= kDotVel; break;
        case SDLK_DOWN: mVelY += kDotVel; break;
        case SDLK_SPACE: mVelY += kDotVel * 2; break;
        case SDLK_LEFT: mVelX -= kDotVel; break;
        case SDLK_RIGHT: mVelX += kDotVel; break;
        }
    }
}

void Dot::move() {
    if (mPosX > kScreenWidth - 20) {
        mPosY = kScreenWidth - kDotWidth;
        mVelX = mVelX * -1.f;
    }
    else if (mPosX < 0) {
        mVelX = mVelX * -1.f;
    }

    if (mPosY > kScreenHeight - kDotHeight) {
        mPosY = kScreenHeight - kDotHeight;
        mVelY *= -0.9f;

        if (fabs(mVelY) < 3.f) {
            mVelY = 0.f;
        }
    }
    else {
        mVelY += accDueToGravity;
    }

    mPosX += mVelX;
    mPosY += mVelY;

};

void Dot::render() {
    gDotTexture.render(static_cast<float>(mPosX), static_cast<float>(mPosY));
};


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
        else {
            if (!SDL_SetRenderVSync(gRenderer, 1)) {
                SDL_Log("vsync error %s\n", SDL_GetError());
                success = false;
            }
            if (!TTF_Init()) {
                SDL_Log("SDL_ttf could not initialize! SDL_ttf error: %s\n", SDL_GetError());
                success = false;
            }
        }
    }
    return success;
};

bool loadMedia() {

    // flag de sucesso
    bool success{ true };

    std::string fontPath = "Inter_28pt-SemiBold.ttf";
    
    gFont = TTF_OpenFont(fontPath.c_str(), 28);

    if (gFont == nullptr) {
        SDL_Log("Não deu para dar load na fonte");
    }
    else {
        SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF };
        
        if (!gTextTexture.loadFromRenderedText("Clique numa tecla para começar o timer", textColor)) {
            SDL_Log("Could not load text texture %s! SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
            success = false;
        }
    }
    
    success &= gUpTexture.loadFromFile("ron.png");
    if (!success) {
        SDL_Log("Não deu para dar load no ron png");
    };
    success &= gDownTexture.loadFromFile("snail.bmp");
    if (!success) {
        SDL_Log("Não deu para dar load no snail bmp");
    };
    success &= gButtonSpriteTexture.loadFromFile("button.png");
    if (!success) {
        SDL_Log("Não deu para dar load no snail bmp");
    };
    success &= gDotTexture.loadFromFile("dot.png");
    if (!success) {
        SDL_Log("Não deu para dar load no snail bmp");
    };

    return success;
};

void close() {
    
    //clean up na textura

    gUpTexture.destroy();
    gDownTexture.destroy();
    gTextTexture.destroy();

    TTF_CloseFont(gFont);
    gFont = nullptr;

    //destruir a janela e o renderer
    SDL_DestroyRenderer(gRenderer);
    gRenderer = nullptr;
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;

    //desligar
    TTF_Quit();
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

            bool vsyncEnabled{ true };
            bool fpsCapEnabled{ false };
            LTimer fpsTimer;
            LTimer capTimer;
            Uint64 renderedFrames = 0;
            Uint64 renderingNS = 0;
            bool resetFps = true;

            Dot dot;

            LTexture* currentTexture = &gUpTexture;

            //LTexture* currentTextTexture = &gTextTexture;

            SDL_Color bgColor = { 0xFF, 0xFF, 0xFF, 0xFF };

            //constexpr int kButtonCount = 1;
            //LButton buttons[kButtonCount];
            //buttons[0].setPosition(0, 0);
            //buttons[0].setVelocity(1, 1);

            //timer

            Uint64 startTime = 0;

            //in memory text stream (nao sei o que é ainda)

            std::stringstream timeText;

            

            //main loop agora

            while ( quit == false ) {


                if (resetFps) {
                    fpsTimer.start();
                    renderedFrames = 0;
                    renderingNS = 0;
                    resetFps = false;
                }

                capTimer.start();
 
                
                while (SDL_PollEvent(&e)) {

                    //testar se o event é do tipo quit

                    if (e.type == SDL_EVENT_QUIT) {

                        //fechar o main loop
                        quit = true;
                    }

                    // isto verifica o cima ou baixo
                    else if (e.type == SDL_EVENT_KEY_DOWN) {
                        //if (e.key.key == SDLK_UP) {
                        //    currentTexture = &gDownTexture;
                        //}
                        //else if (e.key.key == SDLK_DOWN) {
                        //    currentTexture = &gUpTexture;
                        //}

                        if (e.key.key == SDLK_RETURN) {
                            vsyncEnabled = !vsyncEnabled;
                            SDL_SetRenderVSync(gRenderer, (vsyncEnabled ? 1 : SDL_RENDERER_VSYNC_DISABLED));
                        }
                        else if (e.key.key == SDLK_SPACE) {
                            fpsCapEnabled = !fpsCapEnabled;
                            resetFps = true;
                            }



                        // startTime = SDL_GetTicks();
                    }

                    //for (int i = 0; i < kButtonCount; i++) {
                    //    buttons[i].handleEvent(&e);
                    //}

                    dot.handleEvent(e);
                }

                dot.move();
                

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
                //SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                dot.render();




                //if (renderedFrames != 0) {
                //    timeText.str("");
                //    timeText << "Frames per second "
                //        << (vsyncEnabled ? "(VSync) " : "")
                //        << (fpsCapEnabled ? "(Cap) " : "")
                //        << static_cast<double>(renderedFrames) / (static_cast<double>(renderingNS) / 1000000000.0);
                //    SDL_Color textColor = { 0x00, 0x00, 0x00, 0xFF };
                //    gTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor);
                //};

                //render na imagem
                //currentTexture->render(( kScreenWidth - currentTexture->getWidth()) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f);
                //currentTextTexture->render((kScreenWidth - currentTextTexture->getWidth()) / 2.f, (kScreenHeight - currentTextTexture->getHeight()) / 2.f);
                
                //gTextTexture.render((kScreenWidth - gTextTexture.getWidth()) / 2.f, (kScreenHeight - gTextTexture.getHeight()) / 2.f);

                //update no screen
                SDL_RenderPresent(gRenderer);

                renderingNS = fpsTimer.getTicksNS();
                renderedFrames++;

                Uint64 frameNs = capTimer.getTicksNS();

                constexpr Uint64 nsPerFrame = 1000000000 / kScreenFPS;

                if (fpsCapEnabled && frameNs < nsPerFrame) {
                    SDL_DelayNS(nsPerFrame - frameNs);
                }

            }

            //limpar tudo e desligar
            close();

            return exitCode;
        }
    }

    return 0;
}