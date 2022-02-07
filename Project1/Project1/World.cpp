#include "World.hpp"

World::World(Game* game)
	: mSceneGraph(new SceneNode(game))
	, mGame(game)
	, mPlayerAircraft(nullptr)
	, mBackground(nullptr)
	, mWorldBounds(-1.5f, 1.5f, 200.0f, 0.0f)
	, mSpawnPosition(0.f, 0.f)
	, mScrollSpeed(1.0f)
{
}

void World::update(const GameTimer& gt)
{
	mSceneGraph->update(gt);

	XMFLOAT3 LeftEnemyPos = mEnemy[0]->getWorldPosition();
	XMFLOAT3 RightEnemyPos = mEnemy[1]->getWorldPosition();
	XMFLOAT3 velocity = mPlayerAircraft->getVelocity();

	if (LeftEnemyPos.x < mWorldBounds.x || RightEnemyPos.x > mWorldBounds.y)
	{
		velocity.x = -velocity.x;
		mPlayerAircraft->setVelocity(velocity);
		for (int i = 0; i < 2; i++)
		{
			mEnemy[i]->setVelocity(velocity);
		}
	}
}

void World::draw()
{
	mSceneGraph->draw();
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
	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, Aircraft::Player, mGame));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(0, 1, 0);
	mPlayerAircraft->setScale(0.5, 0.5, 0.5);
	mPlayerAircraft->setVelocity(0.5f, 0.0f, 0.0f);
	mSceneGraph->attachChild(std::move(player));

	for (int i = 0; i < 2; i++)
	{
		std::unique_ptr<Aircraft> enemy1 (new Aircraft(Aircraft::Raptor, Aircraft::Enemy, mGame));
		mEnemy[i] = enemy1.get();
		mEnemy[i]->setPosition(mPlayerAircraft->getWorldPosition().x + (i - 0.5f), mPlayerAircraft->getWorldPosition().y, mPlayerAircraft->getWorldPosition().z - 0.25f);
		mEnemy[i]->setScale(0.5, 0.5, 0.5);
		mEnemy[i]->setVelocity(0.5f, 0.0f, 0.0f);
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

