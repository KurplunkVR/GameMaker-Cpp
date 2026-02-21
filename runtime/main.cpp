// GameMaker C++ Runtime
#include "../native/include/IPlatform.h"
#include "../native/include/InputSDL.h"
#include "../native/include/GameEngine.h"
#include <cstdio>
#include <chrono>
#include <thread>

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

	// Create a test room
	auto test_room = std::make_shared<GM::Room>(0, "TestRoom");
	test_room->SetWidth(800);
	test_room->SetHeight(600);
	engine.GetGlobals().GetRoomManager().AddRoom(test_room);
	
	// Create a camera for the room
	auto camera = std::make_shared<GM::Camera>();
	camera->SetID(0);
	camera->SetX(0);
	camera->SetY(0);
	camera->SetWidth(800);
	camera->SetHeight(600);
	test_room->AddCamera(camera);
	test_room->SetActiveCamera(camera);

	// Create a layer for instances
	auto layer = std::make_shared<GM::Layer>(0, "Instances", GM::LayerType::Instances);
	test_room->AddLayer(layer);
	
	// Load the room
	engine.LoadRoom(test_room);

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
