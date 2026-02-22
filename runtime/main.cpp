// GameMaker C++ Runtime
#include "../native/include/IPlatform.h"
#include "../native/include/InputSDL.h"
#include "../native/include/GameEngine.h"
#include "../native/include/AssetLoader.h"
#include <cstdio>
#include <chrono>
#include <thread>
#include <filesystem>

extern "C" IPlatform* CreatePlatform();

int main() {
	printf("[Main] Creating platform...\n");
	IPlatform* platform = CreatePlatform();
	
	printf("[Main] Initializing renderer...\n");
	if (!platform->GetRenderer()->Init(800, 600)) {
		printf("[Main] Renderer initialization failed!\n");
		return 1;
	}

	printf("[Main] Creating game engine...\n");
	GM::GameEngine engine(platform);
	if (!engine.Initialize(800, 600)) {
		printf("[Main] Engine initialization failed!\n");
		return 1;
	}

	// Load Undertale from JSON
	printf("[Main] Loading Undertale game...\n");
	GM::AssetLoader loader;
	std::string undertale_path = "../../tools/undertale_room.json";
	
	printf("[Main] Looking for: %s\n", undertale_path.c_str());
	printf("[Main] Absolute path: %s\n", std::filesystem::absolute(undertale_path).string().c_str());
	printf("[Main] File exists: %d\n", std::filesystem::exists(undertale_path));
	
	if (!std::filesystem::exists(undertale_path)) {
		printf("[Main] ERROR: undertale.json not found at %s\n", undertale_path.c_str());
		printf("[Main] Creating test room instead...\n");
		
		// Fallback: Create a test room
		auto test_room = std::make_shared<GM::Room>(0, "TestRoom");
		test_room->SetWidth(800);
		test_room->SetHeight(600);
		engine.GetGlobals().GetRoomManager().AddRoom(test_room);
		
		auto camera = std::make_shared<GM::Camera>();
		camera->SetID(0);
		camera->SetX(0);
		camera->SetY(0);
		camera->SetWidth(800);
		camera->SetHeight(600);
		test_room->AddCamera(camera);
		test_room->SetActiveCamera(camera);

		auto layer = std::make_shared<GM::Layer>(0, "Instances", GM::LayerType::Instances);
		test_room->AddLayer(layer);
		engine.LoadRoom(test_room);
	} else {
		auto progress = [](int current, int total) {
			printf("[Loader] Progress: %d/%d\n", current, total);
		};
		
		if (!loader.LoadGameFromJSON(undertale_path, progress)) {
			printf("[Main] ERROR: Failed to load undertale.json\n");
			return 1;
		}
		printf("[Main] Undertale loaded successfully!\n");
	}

	printf("[Main] Entering main loop...\n");
	printf("[Main] Press window close button or Ctrl+C to exit\n");
	printf("[Main] Engine: %d FPS target\n", 60);

	InputSDL* input = static_cast<InputSDL*>(platform->GetInput());
	engine.SetTargetFPS(60);
	
	auto last_frame_time = std::chrono::high_resolution_clock::now();
	auto last_stats_time = std::chrono::high_resolution_clock::now();
	
	while (!input->ShouldQuit()) {
		auto current_time = std::chrono::high_resolution_clock::now();
		auto delta_time = std::chrono::duration<double>(current_time - last_frame_time).count();
		last_frame_time = current_time;

		input->PollEvents();
		engine.Tick(delta_time);
		
		// Print stats every second
		auto stats_elapsed = std::chrono::duration<double>(current_time - last_stats_time).count();
		if (stats_elapsed >= 1.0) {
			printf("[Stats] Frame: %d, FPS: %.1f, Delta: %.4fs\n",
				   engine.GetFrameCount(), engine.GetFPS(), delta_time);
			last_stats_time = current_time;
		}
		
		// Sleep to avoid busy waiting
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	engine.Shutdown();
	printf("[Main] Exiting...\n");
	return 0;
}
