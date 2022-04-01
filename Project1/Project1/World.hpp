#pragma once
#include "SceneNode.hpp"
#include "Aircraft.hpp"
#include "SpriteNode.h"
#include "CommandQueue.h"
#include "Command.h"
class World 
{
public:
	explicit World(Game* window);
	void update(const GameTimer& gt);
	void draw();
	CommandQueue& getCommandQueue();
	CommandQueue mCommandQueue;
	void loadTextures(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
					  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
					  std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures);
	
	void loadMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials);
	void buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice,
							Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
							std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries);
	
	/*void buildDescriptorHeaps(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
							  std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures,
							  ComPtr<ID3D12DescriptorHeap> GameSrvDescriptorHeap,
							  UINT GameCbvSrvDescriptorSize
							 );*/
	
	void buildScene();

	Camera* GetCamera() { return &_Camera; }


private:
	enum Layer
	{
		Background,
		Air,
		LayerCount
	};


private:
	// Reference to the Game Backend
	Game* mGame;
	// World Holds the Camera
	Camera _Camera;

	SceneNode* mSceneGraph;
	std::array<SceneNode*, LayerCount> mSceneLayers;

	const static int totalEnemies = 5;

	const static int maxHeight = 15;
	const static int minHeight = 5;

	const static int maxWidth = 15;
	const static int minWidth = 5;

	XMFLOAT4 mWorldBounds;
	XMFLOAT2 mSpawnPosition;
	float mScrollSpeed;
	Aircraft* mPlayerAircraft;
	SpriteNode* mBackground;
	Aircraft* mEnemy[totalEnemies];


};
