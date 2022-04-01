#include "World.hpp"
#include <iostream>

World::World(Game* game)
	: mSceneGraph(new SceneNode(game))
	, mGame(game)
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-10.5f, 10.5f, 200.0f, 0.0f)
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

void World::update(const GameTimer& gt)
{
	mPlayerAircraft->setVelocity(0, 0, 0);
	while (!mCommandQueue.isEmpty())
		mSceneGraph->onCommand(mCommandQueue.pop(), gt);

	mSceneGraph->update(gt);

	if (mPlayerAircraft->getVelocity().y > 0) mPlayerAircraft->setWorldRotation(-1, 0, 0);

	else if (mPlayerAircraft->getVelocity().y < 0) mPlayerAircraft->setWorldRotation(1, 0, 0);

	else if (mPlayerAircraft->getVelocity().x > 0) mPlayerAircraft->setWorldRotation(0, 0, -1);

	else if (mPlayerAircraft->getVelocity().x < 0) mPlayerAircraft->setWorldRotation(0, 0, 1);

	else if (mPlayerAircraft->getVelocity().y == 0 || mPlayerAircraft->getVelocity().x == 0)
		mPlayerAircraft->setWorldRotation(0, 0, 0);	
	
	for (int i = 0; i < totalEnemies; i++)
	{
		if (mEnemy[i]->getVelocity().y > 0) mEnemy[i]->setWorldRotation(-1, 135, 0);

		else if (mEnemy[i]->getVelocity().y < 0) mEnemy[i]->setWorldRotation(1, 135, 0);

		else if (mEnemy[i]->getVelocity().y == 0)
			mEnemy[i]->setWorldRotation(0, 0, 0);
	}


	if (mPlayerAircraft->getWorldPosition().x < -maxWidth)
		mPlayerAircraft->setPosition(-maxWidth, mPlayerAircraft->getWorldPosition().y, mPlayerAircraft->getWorldPosition().z);

	else if (mPlayerAircraft->getWorldPosition().x > maxWidth)
		mPlayerAircraft->setPosition(maxWidth, mPlayerAircraft->getWorldPosition().y, mPlayerAircraft->getWorldPosition().z);

	if (mPlayerAircraft->getWorldPosition().y < minHeight)
		mPlayerAircraft->setPosition(mPlayerAircraft->getWorldPosition().x, minHeight, mPlayerAircraft->getWorldPosition().z);

	else if (mPlayerAircraft->getWorldPosition().y > maxHeight)
		mPlayerAircraft->setPosition(mPlayerAircraft->getWorldPosition().x, maxHeight, mPlayerAircraft->getWorldPosition().z);

	for (int i = 0; i < totalEnemies; i++)
	{
		if (mEnemy[i]->getWorldPosition().x < -maxWidth || mEnemy[i]->getWorldPosition().x > maxWidth)
		{
			mEnemy[i]->setVelocity(-mEnemy[i]->getVelocity().x, mEnemy[i]->getVelocity().y, mEnemy[i]->getVelocity().z);
		}
		
		if (mEnemy[i]->getWorldPosition().y < minHeight || mEnemy[i]->getWorldPosition().y > maxHeight)
		{
			mEnemy[i]->setVelocity(mEnemy[i]->getVelocity().x, -mEnemy[i]->getVelocity().y, mEnemy[i]->getVelocity().z);
		}
	}
}

void World::draw()
{
	mSceneGraph->draw();
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::loadTextures(
						 Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
						 Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList,
						 std::unordered_map<std::string, std::unique_ptr<Texture>>& GameTextures
						)
{
	//Eagle
	auto EagleTex = std::make_unique<Texture>();
	EagleTex->Name = "EagleTex";
	EagleTex->Filename = L"../../Textures/Eagle.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), EagleTex->Filename.c_str(),
		EagleTex->Resource, EagleTex->UploadHeap));

	GameTextures[EagleTex->Name] = std::move(EagleTex);

	//Raptor
	auto RaptorTex = std::make_unique<Texture>();
	RaptorTex->Name = "RaptorTex";
	RaptorTex->Filename = L"../../Textures/Raptor.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), RaptorTex->Filename.c_str(),
		RaptorTex->Resource, RaptorTex->UploadHeap));

	GameTextures[RaptorTex->Name] = std::move(RaptorTex);

	//Desert
	auto DesertTex = std::make_unique<Texture>();
	DesertTex->Name = "DesertTex";
	DesertTex->Filename = L"../../Textures/Desert.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(GameDevice.Get(),
		GameCommandList.Get(), DesertTex->Filename.c_str(),
		DesertTex->Resource, DesertTex->UploadHeap));

	GameTextures[DesertTex->Name] = std::move(DesertTex);
}

void World::loadMaterials(std::unordered_map<std::string, std::unique_ptr<Material>>& GameMaterials)
{
	auto Eagle = std::make_unique<Material>();
	Eagle->Name = "Eagle";
	Eagle->MatCBIndex = 0;
	Eagle->DiffuseSrvHeapIndex = 0;
	Eagle->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Eagle->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Eagle->Roughness = 0.2f;

	GameMaterials["Eagle"] = std::move(Eagle);

	auto Raptor = std::make_unique<Material>();
	Raptor->Name = "Raptor";
	Raptor->MatCBIndex = 1;
	Raptor->DiffuseSrvHeapIndex = 1;
	Raptor->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Raptor->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Raptor->Roughness = 0.2f;

	GameMaterials["Raptor"] = std::move(Raptor);

	auto Desert = std::make_unique<Material>();
	Desert->Name = "Desert";
	Desert->MatCBIndex = 2;
	Desert->DiffuseSrvHeapIndex = 2;
	Desert->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Desert->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
	Desert->Roughness = 0.2f;

	GameMaterials["Desert"] = std::move(Desert);
}

void World::buildShapeGeometry(Microsoft::WRL::ComPtr<ID3D12Device>& GameDevice, 
							   Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& GameCommandList, 
							   std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GameGeometries
							  )
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1, 0, 1, 1);
	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = 0;
	boxSubmesh.BaseVertexLocation = 0;


	std::vector<Vertex> vertices(box.Vertices.size());

	for (size_t i = 0; i < box.Vertices.size(); ++i)
	{
		vertices[i].Pos = box.Vertices[i].Position;
		vertices[i].Normal = box.Vertices[i].Normal;
		vertices[i].TexC = box.Vertices[i].TexC;
	}

	std::vector<std::uint16_t> indices = box.GetIndices16();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		GameCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(GameDevice.Get(),
		GameCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;

	GameGeometries[geo->Name] = std::move(geo);
}

void World::buildScene()
{
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mGame));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 1, -10);
	mPlayerAircraft->setScale(3.0, 3.0, 3.0);
	mPlayerAircraft->setVelocity(2.5f, 2.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	for (int i = 0; i < totalEnemies; i++)
	{
		std::unique_ptr<Aircraft> enemy1 (new Aircraft(Aircraft::Raptor, mGame));
		mEnemy[i] = enemy1.get();
		mEnemy[i]->setPosition(mPlayerAircraft->getWorldPosition().x * i, (rand() % 5) + 5, mPlayerAircraft->getWorldPosition().z + 10.0f);
		mEnemy[i]->setScale(3.0, 3.0, 3.0);
		mEnemy[i]->setVelocity(2.f * i + i, 2.0f, 0.0f);
		mSceneGraph->attachChild(std::move(enemy1));
	}

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(mGame));
	mBackground = backgroundSprite.get();
	mBackground->setPosition(0, 0, 0.0);
	mBackground->setScale(200.0, 1.0, 200.0);
	mBackground->setVelocity(0, 0, -mScrollSpeed);
	mSceneGraph->attachChild(std::move(backgroundSprite));

	mSceneGraph->build();
}

