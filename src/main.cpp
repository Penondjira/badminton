#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <ostream>
#include <random>
#include <string>

int main(int argc, char* argv[]) {
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (result != 0) {
		std::cout << "SDL_Init failed" << std::endl;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 2048) < 0) {
		std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}

	Mix_Chunk* sound = Mix_LoadWAV("../assets/hit.wav");
		if (sound == nullptr) {
		std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
	}

	SDL_Window* win = SDL_CreateWindow("Badminton", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 608, 416, SDL_WINDOW_SHOWN);
	if (win == nullptr) {
		std::cout << "SDL_CreateWindow failed" << std::endl;
		SDL_Quit();
	}

	SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr) {
		std::cout << "SDL_CreateRenderer failed" << std::endl;
		SDL_DestroyWindow(win);
		SDL_Quit();
	}

	int imgFlags = IMG_INIT_PNG;
	int initializedFlags = IMG_Init(imgFlags);
	if (initializedFlags != imgFlags) {
		std::cout << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	result = TTF_Init();
	if (result == -1) {
		std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	TTF_Font* font = TTF_OpenFont("../assets/DejaVuSans.ttf", 24);
	if (font == nullptr) {
		std::cout << "TTF_OpenFont error: " << TTF_GetError() << std::endl;
		TTF_Quit();
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}
	
	const char* badmintonCourtPath = "../assets/court.png";
	SDL_Surface* courtS = IMG_Load(badmintonCourtPath);
	if (courtS == nullptr) {
		std::cout << "Unable to load image! IMG_Error: " << IMG_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	SDL_Texture* courtT = SDL_CreateTextureFromSurface(ren, courtS);
	SDL_FreeSurface(courtS);
	if (courtT == nullptr) {
		std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	const char* racketPath = "../assets/racket.png";
	SDL_Surface* racketS = IMG_Load(racketPath);
	if (racketS == nullptr) {
		std::cout << "Unable to load image! IMG_Error: " << IMG_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	SDL_Texture* racketT = SDL_CreateTextureFromSurface(ren, racketS);
	SDL_FreeSurface(racketS);
	if (racketT == nullptr) {
		std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	const char* birdiePath = "../assets/birdie.png";
	SDL_Surface* birdieS = IMG_Load(birdiePath);
	if (birdieS == nullptr) {
		std::cout << "Unable to load image! IMG_Error: " << IMG_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	SDL_Texture* birdieT = SDL_CreateTextureFromSurface(ren, birdieS);
	SDL_FreeSurface(birdieS);
	if (birdieT == nullptr) {
		std::cout << "Unable to create texture! SDL_Error: " << SDL_GetError() << std::endl;
		IMG_Quit();
		SDL_DestroyWindow(win);
		SDL_DestroyRenderer(ren);
		SDL_Quit();
	}

	int courtW = 608;
	int courtH = 416;
	SDL_Rect courtCopy = {0, 0, courtW, courtH};
	SDL_Rect courtDestination = {0, 0, courtW, courtH};

	int birdieW = 31;
	int birdieH = 45;
	SDL_Rect birdieCopy = {0, 0, birdieW, birdieH};
	int birdieX = 300;
	int birdieY = 50;
	int birdieSpawnMinX = 100;
	int birdieSpawnMaxX = 508;
	int birdieVel = 2;

	int racketW = 47;
	int racketH = 110;
	int racketHeadCenterX = 24;
	int racketHeadCenterY = 31;
	int racketHeadLeftX = 2;
	int racketHeadRightX = 44;
	int racketHeadTopY = 3;
	int racketHeadBotY = 60;
	SDL_Rect racketCopy = {0, 0, racketW, racketH};
	int mouseX, mouseY;

	std::random_device rd;
	std::mt19937 gen = std::mt19937(rd());
	std::uniform_int_distribution<> distr(birdieSpawnMinX, birdieSpawnMaxX);

	int score = 0;

	bool gameOver = false;

	bool quit = false;
	SDL_Event event;

	while (!quit) {
		int foundEvent = SDL_PollEvent(&event);
		while (foundEvent != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
			foundEvent = SDL_PollEvent(&event);
		}
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, courtT, &courtCopy, &courtDestination);


		SDL_Rect birdieDestination = {birdieX, birdieY, birdieW, birdieH};
		SDL_RenderCopy(ren, birdieT, &birdieCopy, &birdieDestination);
		birdieY += birdieVel;

		if (birdieX + birdieW > mouseX - racketHeadCenterX + racketHeadLeftX
			&& birdieX < mouseX - racketHeadCenterX + racketHeadRightX
			&& birdieY < mouseY - racketHeadCenterY + racketHeadBotY
			&& birdieY + birdieH > mouseY - racketHeadCenterY + racketHeadTopY) {
			std::cout << "hit" << std::endl;
			birdieY = 50;
			birdieX = distr(gen);
			score++;
			birdieVel++;
			Mix_PlayChannel(-1, sound, 0);
		}

		if (birdieY > courtH) {
			gameOver = true;
		}

		const char* scorePrefix = "Score: ";
		std::string scoreDisplayStr = std::string(scorePrefix) + std::to_string(score);
		const char* scoreDisplay = scoreDisplayStr.c_str();
		SDL_Color color = {0, 0, 0};
		SDL_Surface* scoreS = TTF_RenderText_Solid(font, scoreDisplay, color);
		SDL_Texture* scoreT = SDL_CreateTextureFromSurface(ren, scoreS);
		SDL_FreeSurface(scoreS);
		int texW = 0;
		int texH = 0;
		SDL_QueryTexture(scoreT, nullptr, nullptr, &texW, &texH);
		SDL_Rect scoreCopy = {0, 0, texW, texH};
		SDL_Rect scoreDestination = {0, 0, texW, texH};
		SDL_RenderCopy(ren, scoreT, &scoreCopy, &scoreDestination);

		if (gameOver) {
			const char* finalScorePrefix = "Final Score: ";
			std::string finalScoreDisplayStr = std::string(finalScorePrefix) + std::to_string(score);
			const char* finalScoreDisplay = finalScoreDisplayStr.c_str();
			SDL_Color color = {0, 0, 0};
			SDL_Surface* finalScoreS = TTF_RenderText_Solid(font, finalScoreDisplay, color);
			SDL_Texture* finalScoreT = SDL_CreateTextureFromSurface(ren, finalScoreS);
			SDL_FreeSurface(finalScoreS);
			int texW = 0;
			int texH = 0;
			SDL_QueryTexture(finalScoreT, nullptr, nullptr, &texW, &texH);
			SDL_Rect finalScoreCopy = {0, 0, texW, texH};
			SDL_Rect finalScoreDestination = {350, 300, texW, texH};
			SDL_RenderCopy(ren, finalScoreT, &finalScoreCopy, &finalScoreDestination);
		}

		SDL_GetMouseState(&mouseX, &mouseY);
		SDL_Rect racketDestination;
		if (gameOver == false) {
			racketDestination = {mouseX - racketHeadCenterX, mouseY - racketHeadCenterY, racketW, racketH};
		}
		else {
			racketDestination = {-300, -300, racketW, racketH};
		}
		SDL_RenderCopy(ren, racketT, &racketCopy, &racketDestination);

		SDL_RenderPresent(ren);
		SDL_Delay(16);
	}
	Mix_FreeChunk(sound);
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}
