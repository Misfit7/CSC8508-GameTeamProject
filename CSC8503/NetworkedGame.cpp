#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"

#define COLLISION_MSG 30

NetworkedGame* NetworkedGame::networkInstance = nullptr;

struct MessagePacket : public GamePacket {
    short playerID;
    short messageID;

    MessagePacket() {
        type = Message;
        size = sizeof(short) * 2;
    }
};

NetworkedGame::NetworkedGame() {
    thisServer = nullptr;
    thisClient = nullptr;

    NetworkBase::Initialise();
    servertimeToNextPacket = 0.0f;
    clienttimeToNextPacket = 0.0f;
    packetsToSnapshot = 0;

    playerNum = -1;

    networkInstance = this;
}

NetworkedGame::~NetworkedGame() {
    delete thisServer;
    delete thisClient;
}

void NetworkedGame::StartAsServer() {
    thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

    thisServer->RegisterPacketHandler(Delta_State, this);
    thisServer->RegisterPacketHandler(Full_State, this);
    thisServer->RegisterPacketHandler(Received_State, this);
    thisServer->RegisterPacketHandler(Spawn_Player, this);

    StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
    thisClient = new GameClient();
    thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());

    thisClient->RegisterPacketHandler(Delta_State, this);
    thisClient->RegisterPacketHandler(Full_State, this);
    thisClient->RegisterPacketHandler(Player_Connected, this);
    thisClient->RegisterPacketHandler(Player_Disconnected, this);
    thisClient->RegisterPacketHandler(Update_Objects, this);
    thisClient->RegisterPacketHandler(Client_Num, this);
    thisClient->RegisterPacketHandler(Win_Lose, this);
    thisClient->RegisterPacketHandler(Restart_Game, this);

    SpawnCarriage();
}

void NetworkedGame::UpdateGame(float dt) {
    switch (world->GetGameState())
    {
    case LOADING:
        UpdateLoading(dt);
        break;
    case PLAYING:
        TutorialGame::UpdateGame(dt);
        break;
    case SERVERPLAYING:
        UpdateNetworkedPlaying(dt);
        break;
    case CLIENTPLAYING:
        UpdateNetworkedPlaying(dt);
        break;
    case PAUSED:
        UpdatePaused(dt);
        break;
    case FAILURE:
        UpdateFailure(dt);
        break;
    case FINISH:
        UpdateFinish(dt);
        break;
    case MENU:
        UpdateMenu(dt);
        break;
    case CHOOSESERVER:
        UpdateChooseServer(dt);
        break;
    case EXIT:
        isExit = true;
        break;
    default:
        break;
    }
    //if (thisServer)
    //    std::cout << "Server State: " << world->GetGameState() << std::endl;
    //else
    //    std::cout << "Client State: " << world->GetGameState() << std::endl;
}

void NetworkedGame::UpdateNetworkedPlaying(float dt) {
    if (thisServer)
        servertimeToNextPacket -= dt;
    else
        clienttimeToNextPacket -= dt;
    if (thisServer && servertimeToNextPacket < 0) {
        UpdateAsServer(dt);
        servertimeToNextPacket += 1.0f / 60.0f; //30hz server update
    }
    else if (thisClient && clienttimeToNextPacket < 0) {
        UpdateAsClient(dt);
        clienttimeToNextPacket += 1.0f / 60.0f; //60hz client update
    }

    if (!inSelectionMode) {
        if (cameraMode == 1) {
            CameraUpdate();
        }
        else {
            world->GetMainCamera().UpdateCamera(dt);
        }
    }

    DrawPad();
    UpdateKeys();

    playtime += dt;
    audio->Update();
    if (thisServer) world->UpdateWorld(dt);
    else if (thisClient) world->UpdateWorld(dt, true);
    renderer->Update(dt);
    renderer->GetUI()->Update(dt); //UI
    physics->Update(dt);
    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void NetworkedGame::UpdateKeys() {
    if (Window::GetKeyboard()->KeyPressed(KeyCodes::ESCAPE)) {
        if (world->GetGameState() == GameState::SERVERPLAYING ||
            world->GetGameState() == GameState::CLIENTPLAYING ||
            world->GetGameState() == GameState::PLAYING)
            world->SetGameState(GameState::PAUSED);
        else if (world->GetGameState() == GameState::PAUSED) {
            if (IsServer())
                world->SetGameState(GameState::SERVERPLAYING);
            else if (IsClient())
                world->SetGameState(GameState::CLIENTPLAYING);
            else
                world->SetGameState(GameState::PLAYING);
            Window::GetWindow()->ShowOSPointer(false);
            Window::GetWindow()->LockMouseToWindow(true);
        }
    }

    if (world->GetGameState() == GameState::SERVERPLAYING ||
        world->GetGameState() == GameState::CLIENTPLAYING) {
        if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
            cameraMode = 1;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
            cameraMode = 2;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::P)) {
            usePad = !usePad;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::M)) {
            success = true;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::E)) {
            playerUI = !playerUI;
        }

        if (Window::GetKeyboard()->KeyPressed(KeyCodes::B)) {
            showDebug = !showDebug;
        }
    }
}

void NetworkedGame::UpdatePaused(float dt)
{
    if (thisServer)
        servertimeToNextPacket -= dt;
    else
        clienttimeToNextPacket -= dt;
    if (thisServer && servertimeToNextPacket < 0) {
        UpdateAsServer(dt);
        servertimeToNextPacket += 1.0f / 60.0f; //30hz server update
    }
    else if (thisClient && clienttimeToNextPacket < 0) {
        UpdateAsClient(dt);
        clienttimeToNextPacket += 1.0f / 60.0f; //60hz client update
    }

    if (!inSelectionMode) {
        if (cameraMode == 1) {
            CameraUpdate();
        }
        else {
            world->GetMainCamera().UpdateCamera(dt);
        }
    }

    UpdateKeys();
    audio->Update();
    if (thisServer) world->UpdateWorld(dt);
    else if (thisClient) world->UpdateWorld(dt, true);
    renderer->Update(dt);
    renderer->GetUI()->Update(dt); //UI
    renderer->Render();
    Debug::UpdateRenderables(dt);
}

void NetworkedGame::UpdateFailure(float dt) {
    if (thisServer)
        servertimeToNextPacket -= dt;
    else
        clienttimeToNextPacket -= dt;

    if (thisServer && servertimeToNextPacket < 0) {
        ServerBroadcast();
        if (!winFlag) {
            BoolPacket losePacket;
            losePacket.flag = false;
            losePacket.type = Win_Lose;
            thisServer->SendGlobalPacket(losePacket);
            winFlag = true;
        }
        servertimeToNextPacket += 1.0f / 60.0f;
    }
    else if (thisClient && clienttimeToNextPacket < 0) {
        thisClient->UpdateClient();
        if (restartFlag) {
            InitGameWorld(true);
            SpawnCarriage();
            spawn = true;
            spawnFlag = true;
            restartFlag = false;
            world->SetGameState(GameState::CLIENTPLAYING);
        }
        clienttimeToNextPacket += 1.0f / 60.0f; //60hz client update
    }

    audio->Update();
    renderer->Update(dt);
    renderer->GetUI()->Update(dt); //UI
    renderer->Render();
    Debug::UpdateRenderables(dt);

    //std::cout << "TrainLastFullID: " << train->GetNetworkObject()->GetLastFullState().GetStateID() << std::endl;
}

void NetworkedGame::UpdateFinish(float dt) {
    if (thisServer)
        servertimeToNextPacket -= dt;
    else
        clienttimeToNextPacket -= dt;

    if (thisServer && servertimeToNextPacket < 0) {
        ServerBroadcast();
        if (!winFlag) {
            BoolPacket winPacket;
            winPacket.flag = true;
            winPacket.type = Win_Lose;
            thisServer->SendGlobalPacket(winPacket);
            winFlag = true;
        }
        servertimeToNextPacket += 1.0f / 60.0f;
    }
    else if (thisClient && clienttimeToNextPacket < 0) {
        thisClient->UpdateClient();
        if (restartFlag) {
            level = 1;
            InitGameWorld(true, 1);
            SpawnCarriage();
            spawn = true;
            spawnFlag = true;
            restartFlag = false;
            world->SetGameState(GameState::CLIENTPLAYING);
        }
        clienttimeToNextPacket += 1.0f / 60.0f; //60hz client update
    }

    audio->Update();
    renderer->Update(dt);
    renderer->GetUI()->Update(dt); //UI
    renderer->Render();
    Debug::UpdateRenderables(dt);

    //std::cout << "TrainLastFullID: " << train->GetNetworkObject()->GetLastFullState().GetStateID() << std::endl;
}

void NetworkedGame::UpdateAsServer(float dt) {
    if (thisServer->GetSpawnFlag()) {
        SpawnPlayer();
        SynchronizeGameObjects();
    }

    if (spawnFlag) {
        SpawnPlayer();
    }

    if (restartFlag) {
        BoolPacket restartPacket;
        restartPacket.flag = true;
        restartPacket.type = Restart_Game;
        thisServer->SendGlobalPacket(restartPacket);
        restartFlag = false;
    }

    packetsToSnapshot--;
    if (packetsToSnapshot < 0) {
        BroadcastSnapshot(false);
        packetsToSnapshot = 5;
    }
    else {
        BroadcastSnapshot(true);
    }
    thisServer->UpdateServer();
    if (failure) {
        world->SetGameState(GameState::FAILURE);
        TutorialGame::GetGame()->GetAudio()->PlayFailure();
    }
    if (success) {
        if (level < 5) {
            if (!winFlag) {
                BoolPacket winPacket;
                winPacket.flag = true;
                winPacket.type = Win_Lose;
                thisServer->SendGlobalPacket(winPacket);
                winFlag = true;
            }
            renderer->GetUI()->SetSuccess(true);
            level++;
            InitGameWorld(true, level);
            StartLevel();
        }
        else {
            world->SetGameState(GameState::FINISH);
            TutorialGame::GetGame()->GetAudio()->PlayWin();
        }
    }

    if (thisServer->GetRemovePlayer()) {
        switch (thisServer->GetRemovePlayer()) {
        case 2:
            world->RemoveGameObject(player2);
            playerRemoved = true;
            removedPlayerNetworkID = 2;
            removePlayerTag = 9;
            thisServer->SetRemovePlayer(0);
            break;
        case 3:
            world->RemoveGameObject(player3);
            playerRemoved = true;
            removedPlayerNetworkID = 3;
            removePlayerTag = 9;
            thisServer->SetRemovePlayer(0);
            break;
        case 4:
            world->RemoveGameObject(player4);
            playerRemoved = true;
            removedPlayerNetworkID = 4;
            removePlayerTag = 9;
            thisServer->SetRemovePlayer(0);
            break;
        default:
            break;
        }
    }

    if (showDebug) {
        if (thisServer->ConnectDebug()) {
            connectCounter = 3.0f;
            showConnect = true;
            thisServer->SetConnectDebug(false);
        }
        if (thisServer->DisconnectDebug()) {
            disconnectCounter = 3.0f;
            showDisconnect = true;
            thisServer->SetDisconnectDebug(false);
        }
        if (showConnect) {
            connectCounter -= dt;
            if (connectCounter > 0) {
                Debug::Print("Server: New client connected", Vector2(0, 25), Debug::BLUE);
            }
            else {
                showConnect = false;
            }
        }
        if (showDisconnect) {
            disconnectCounter -= dt;
            if (disconnectCounter > 0) {
                Debug::Print("Server: A client has disconnected", Vector2(0, 30), Debug::BLUE);
            }
            else {
                showDisconnect = false;
            }
        }
    }

    //std::cout << "Player1: " << player->GetTransform().GetPosition().x << " " <<
    //	player->GetTransform().GetPosition().y << " " <<
    //	player->GetTransform().GetPosition().z << " " << std::endl;
    //std::cout << "TrainLastFullID: " << train->GetNetworkObject()->GetLastFullState().GetStateID() << std::endl;
}

void NetworkedGame::UpdateAsClient(float dt) {
    ClientPacket newPacket;

    newPacket.playerNum = playerNum;
    if (Window::GetKeyboard()->KeyHeld(KeyCodes::W))
        newPacket.buttonstates[0] = 1;

    if (Window::GetKeyboard()->KeyHeld(KeyCodes::S))
        newPacket.buttonstates[1] = 1;

    if (Window::GetKeyboard()->KeyHeld(KeyCodes::A))
        newPacket.buttonstates[2] = 1;

    if (Window::GetKeyboard()->KeyHeld(KeyCodes::D))
        newPacket.buttonstates[3] = 1;

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::R))
        newPacket.buttonstates[4] = 1;

    if (Window::GetKeyboard()->KeyHeld(KeyCodes::SPACE))
        newPacket.buttonstates[5] = 1;

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE))
        newPacket.buttonstates[6] = 1;

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::F))
        newPacket.buttonstates[7] = 1;

    if (Window::GetKeyboard()->KeyPressed(KeyCodes::SHIFT))
        newPacket.buttonstates[8] = 1;

    thisClient->SendPacket(newPacket);

    thisClient->UpdateClient();

    if (failure) {
        world->SetGameState(GameState::FAILURE);
        TutorialGame::GetGame()->GetAudio()->PlayFailure();
    }
    if (success) {
        if (level < 5) {
            renderer->GetUI()->SetSuccess(true);
            level++;
            InitGameWorld(true, level);
            SpawnCarriage();
            spawn = true;
            spawnFlag = true;
        }
        else {
            world->SetGameState(GameState::FINISH);
            TutorialGame::GetGame()->GetAudio()->PlayWin();
        }
    }


    if (spawnFlag) {
        SpawnPacket spawnPacket;
        spawnPacket.flag = true;
        spawnPacket.num = spawnNum;
        thisClient->SendPacket(spawnPacket);
        spawnFlag = false;
    }

    if (spawn)
        SpawnPlayer();

    if (!objectSpawned)
        SpawnGameObjects();

    UpdateGameObjects();

    if (showDebug) {
        if (showConnect) {
            connectCounter -= dt;
            if (connectCounter > 0) {
                Debug::Print("Client: Connected to server", Vector2(0, 25), Debug::BLUE);
            }
            else {
                showConnect = false;
            }
        }
    }

    //std::cout << "TrainLastFullID: " << train->GetNetworkObject()->GetLastFullState().GetStateID() << std::endl;
    //if (player)
    //    std::cout << "Player: " << player->GetTransform().GetPosition().x << " " << player->GetTransform().GetPosition().y << " " << player->GetTransform().GetPosition().z << " " << std::endl;
}

void NetworkedGame::ServerBroadcast() {
    packetsToSnapshot--;
    if (packetsToSnapshot < 0) {
        BroadcastSnapshot(false);
        packetsToSnapshot = 5;
    }
    else {
        BroadcastSnapshot(true);
    }
    thisServer->UpdateServer();
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
    std::vector<GameObject*>::const_iterator first;
    std::vector<GameObject*>::const_iterator last;

    world->GetObjectIterators(first, last);

    for (auto i = first; i != last; ++i) {
        NetworkObject* o = (*i)->GetNetworkObject();
        if (!o || !o->IsUpdate()) {
            continue;
        }
        //TODO - you'll need some way of determining
        //when a player has sent the server an acknowledgement
        //and store the lastID somewhere. A map between player
        //and an int could work, or it could be part of a 
        //NetworkPlayer struct. 
        int playerState = 0;
        GamePacket* newPacket = nullptr;
        if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
            thisServer->SendGlobalPacket(*newPacket);
            delete newPacket;
        }
    }
    if (treeCut) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = plankNetworkID;
        updatePacket.worldID = treeWorldID;
        updatePacket.tag = treeCutTag;
        thisServer->SendGlobalPacket(updatePacket);
        treeCut = false;
    }
    if (rockDug) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = stoneNetworkID;
        updatePacket.worldID = rockWorldID;
        updatePacket.tag = rockDugTag;
        thisServer->SendGlobalPacket(updatePacket);
        rockDug = false;
    }
    if (materialUpdating) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = removedPlankNetworkID;
        updatePacket.networkID2 = removedStoneNetworkID;
        updatePacket.tag = materialUpdatingTag;
        thisServer->SendGlobalPacket(updatePacket);
        materialUpdating = false;
    }
    if (railProduced) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = railNetworkID;
        updatePacket.tag = railProducedTag;
        thisServer->SendGlobalPacket(updatePacket);
        railProduced = false;
    }
    if (updateRail) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = updateRailNetworkID;
        updatePacket.tag = updateRailTag;
        thisServer->SendGlobalPacket(updatePacket);
        updateRail = false;
    }
    if (bridgeBuilt) {
        UpdatePacket updatePacket;

        updatePacket.worldID = waterWorldID;
        updatePacket.tag = bridgeBuiltTag;
        thisServer->SendGlobalPacket(updatePacket);
        bridgeBuilt = false;
    }
    if (playerRemoved) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = removedPlayerNetworkID;
        updatePacket.tag = removePlayerTag;
        thisServer->SendGlobalPacket(updatePacket);
        playerRemoved = false;
    }
}

void NetworkedGame::ClientSend(bool deltaFrame) {
    int playerState = 0;
    GamePacket* newPacket = nullptr;
    if (localPlayer->GetNetworkObject()->WritePacket(&newPacket, deltaFrame, playerState)) {
        thisClient->SendPacket(*newPacket);
        delete newPacket;
    }
}

void NetworkedGame::UpdateMinimumState() {
    //Periodically remove old data from the server
    int minID = INT_MAX;
    int maxID = 0; //we could use this to see if a player is lagging behind?

    for (auto i : stateIDs) {
        minID = std::min(minID, i.second);
        maxID = std::max(maxID, i.second);
    }
    //every client has acknowledged reaching at least state minID
    //so we can get rid of any old states!
    std::vector<GameObject*>::const_iterator first;
    std::vector<GameObject*>::const_iterator last;
    world->GetObjectIterators(first, last);

    for (auto i = first; i != last; ++i) {
        NetworkObject* o = (*i)->GetNetworkObject();
        if (!o) {
            continue;
        }
        o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
    }
}

void NetworkedGame::SpawnPlayer() {
    if (thisServer) {
        if (!spawnFlag) {
            if (thisServer->GetClientCount() == 1) {
                player2->SetSpawned(true);
                player2->GetTransform().SetPosition(player2Position);
                thisServer->SetSpawnFlag(false);
            }
            else if (thisServer->GetClientCount() == 2) {
                player3->SetSpawned(true);
                player3->GetTransform().SetPosition(player3Position);
                thisServer->SetSpawnFlag(false);
            }
            else if (thisServer->GetClientCount() == 3) {
                player4->SetSpawned(true);
                player4->GetTransform().SetPosition(player4Position);
                thisServer->SetSpawnFlag(false);
            }

            NumPacket numPacket;

            numPacket.num = thisServer->GetClientCount() + 1;
            thisServer->SendGlobalPacket(numPacket);
        }
        else {
            if (spawnNum == 2) {
                player2->SetSpawned(true);
                player2->GetTransform().SetPosition(player2Position);
                spawnNum = 0;
                spawnFlag = false;
            }
            if (spawnNum == 3) {
                player3->SetSpawned(true);
                player3->GetTransform().SetPosition(player3Position);
                spawnNum = 0;
                spawnFlag = false;
            }
            if (spawnNum == 4) {
                player4->SetSpawned(true);
                player4->GetTransform().SetPosition(player4Position);
                spawnNum = 0;
                spawnFlag = false;
            }
        }
    }
    else if (thisClient) {
        if (spawnNum == 2) {
            player2->SetSpawned(true);
            player2->GetTransform().SetPosition(player2Position);
            if (playerNum == -1 || playerNum == 2) {
                playerNum = 2;
                localPlayer = player2;
            }
            spawn = false;
        }
        if (spawnNum == 3) {
            player3->SetSpawned(true);
            player3->GetTransform().SetPosition(player3Position);
            if (playerNum == -1 || playerNum == 3) {
                playerNum = 3;
                localPlayer = player3;
            }
            spawn = false;
        }
        if (spawnNum == 4) {
            player4->SetSpawned(true);
            player4->GetTransform().SetPosition(player4Position);
            if (playerNum == -1 || playerNum == 4) {
                playerNum = 4;
                localPlayer = player4;
            }
            spawn = false;
        }
    }
}

void NetworkedGame::SpawnCarriage() {
    train->SetSpawned(true);
    carriage1 = (MaterialCarriage*)(train->AddCarriage(21, true));
    carriage2 = (ProduceCarriage*)(train->AddCarriage(22, true));
    carriage3 = (WaterCarriage*)(train->AddCarriage(23, true));
    carriage1->SetProduceCarriage(carriage2);
    carriage2->SetMaterialCarriage(carriage1);
}

void NetworkedGame::SynchronizeGameObjects() {
    std::vector<GameObject*>::const_iterator first1;
    std::vector<GameObject*>::const_iterator last1;
    world->GetObjectIterators(first1, last1);

    for (auto i = first1; i != last1; ++i) {
        if ((*i)->GetTypeID() == 5 || (*i)->GetTypeID() == 6 ||
            ((*i)->GetTypeID() == 7 && !(*i)->GetFlag1())) {
            UpdatePacket updatePacket;

            updatePacket.typeID = (*i)->GetTypeID();
            updatePacket.networkID1 = (*i)->GetNetworkObject()->GetNetworkID();
            updatePacket.tag = 5;
            thisServer->SendGlobalPacket(updatePacket);
            //std::cout << "Server: updatePacket" << std::endl;
        }
    }

    std::vector<int>::const_iterator first2;
    std::vector<int>::const_iterator last2;
    world->GetRemovedObjectIterators(first2, last2);

    for (auto i = first2; i != last2; ++i) {
        UpdatePacket updatePacket;

        updatePacket.networkID1 = *i;
        updatePacket.tag = 6;
        thisServer->SendGlobalPacket(updatePacket);
    }
}

void NetworkedGame::SpawnGameObjects() {
    if (newObjectTag == 5) {
        for (int i = 0; i < objectType.size(); ++i) {
            switch (objectType[i]) {
            case 5:
                AddPlankToWorld(Vector3(0, -2000, 0), true, objectNetworkID[i]);
                break;
            case 6:
                AddStoneToWorld(Vector3(0, -2000, 0), true, objectNetworkID[i]);
                break;
            case 7:
                AddRailToWorld(Vector3(0, -2000, 0), true, objectNetworkID[i]);
                break;
            }
        }
        newObjectTag = 0;
    }
    if (removeObjectTag == 6) {
        for (int i = 0; i < removedObjectNetworkID.size(); ++i) {
            world->RemoveGameObject(removedObjectNetworkID[i], true);
        }
        removeObjectTag = 0;
        objectSpawned = true;
    }
}

void NetworkedGame::UpdateGameObjects() {
    if (treeCutTag == 1) {
        AddPlankToWorld(Vector3(0, -2000, 0), true, plankNetworkID);
        world->RemoveGameObject(treeWorldID);
        treeCutTag = 0;
    }
    else if (rockDugTag == 2) {
        AddStoneToWorld(Vector3(0, -2000, 0), true, stoneNetworkID);
        world->RemoveGameObject(rockWorldID);
        rockDugTag = 0;
    }
    else if (materialUpdatingTag == 3) {
        world->RemoveGameObject(removedPlankNetworkID, true);
        world->RemoveGameObject(removedStoneNetworkID, true);
        materialUpdatingTag = 0;
    }
    else if (railProducedTag == 4) {
        AddRailToWorld(Vector3(0, -3000, 0), true, railNetworkID);
        railProducedTag = 0;
    }
    else if (updateRailTag == 7) {
        std::vector<GameObject*>::const_iterator first;
        std::vector<GameObject*>::const_iterator last;

        world->GetObjectIterators(first, last);

        for (auto i = first; i != last; ++i) {
            if ((*i)->GetTypeID() == 7) {
                if ((*i)->GetNetworkObject()->GetNetworkID() == updateRailNetworkID) {
                    (*i)->GetRenderObject()->SetMesh(railTurnMesh);
                    (*i)->GetRenderObject()->SetDefaultTexture(railTurnTex);
                    (*i)->GetRenderObject()->SetBumpTexture(railTurnBumpTex);
                }
            }
        }
        updateRailTag = 0;
    }
    else if (bridgeBuiltTag == 8) {
        world->RemoveGameObject(waterWorldID);
        bridgeBuiltTag = 0;
    }
    else if (removePlayerTag == 9) {
        world->RemoveGameObject(removedPlankNetworkID);
        removePlayerTag = 0;
    }
}

void NetworkedGame::StartLevel() {
    train->SetSpawned(true);
    train->GetTransform().SetPosition(trainPosition);
    carriage1 = (MaterialCarriage*)(train->AddCarriage(21, true));
    carriage2 = (ProduceCarriage*)(train->AddCarriage(22, true));
    carriage3 = (WaterCarriage*)(train->AddCarriage(23, true));
    carriage1->SetProduceCarriage(carriage2);
    carriage2->SetMaterialCarriage(carriage1);
    player->SetSpawned(true);
    player->GetTransform().SetPosition(player1Position);
    localPlayer = player;
    playerNum = 1;
    pickaxe->SetSpawned(true);
    pickaxe->GetTransform().SetPosition(pickaxePosition);
    axe->SetSpawned(true);
    axe->GetTransform().SetPosition(axePosition);
    bucket->SetSpawned(true);
    bucket->GetTransform().SetPosition(bucketPosition);
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
    if (thisClient) {
        if (payload->type == Full_State) {
            std::vector<GameObject*>::const_iterator first;
            std::vector<GameObject*>::const_iterator last;
            world->GetObjectIterators(first, last);
            for (auto i = first; i != last; ++i) {
                NetworkObject* o = (*i)->GetNetworkObject();
                if (!o || !o->IsUpdate()) {
                    continue;
                }
                o->ReadPacket(*payload);
            }
        }
        else if (payload->type == Update_Objects) {
            UpdatePacket* updatePacket = (UpdatePacket*)payload;

            if (updatePacket->tag == 1) {
                treeCutTag = updatePacket->tag;
                plankNetworkID = updatePacket->networkID1;
                treeWorldID = updatePacket->worldID;
            }
            else if (updatePacket->tag == 2) {
                rockDugTag = updatePacket->tag;
                stoneNetworkID = updatePacket->networkID1;
                rockWorldID = updatePacket->worldID;
            }
            else if (updatePacket->tag == 3) {
                materialUpdatingTag = updatePacket->tag;
                removedPlankNetworkID = updatePacket->networkID1;
                removedStoneNetworkID = updatePacket->networkID2;
            }
            else if (updatePacket->tag == 4) {
                railProducedTag = updatePacket->tag;
                railNetworkID = updatePacket->networkID1;
            }
            else if (updatePacket->tag == 5) {
                newObjectTag = updatePacket->tag;
                objectType.push_back(updatePacket->typeID);
                objectNetworkID.push_back(updatePacket->networkID1);
                //std::cout << "Client: updatePacket" << std::endl;
            }
            else if (updatePacket->tag == 6) {
                removeObjectTag = updatePacket->tag;
                removedObjectNetworkID.push_back(updatePacket->networkID1);
            }
            else if (updatePacket->tag == 7) {
                updateRailTag = updatePacket->tag;
                updateRailNetworkID = updatePacket->networkID1;
            }
            else if (updatePacket->tag == 8) {
                bridgeBuiltTag = updatePacket->tag;
                waterWorldID = updatePacket->worldID;
            }
            else if (updatePacket->tag == 9) {
                removePlayerTag = updatePacket->tag;
                removedPlayerNetworkID = updatePacket->networkID1;
            }
        }
        else if (payload->type == Client_Num) {
            NumPacket* numPacket = (NumPacket*)payload;

            spawnNum = numPacket->num;
            spawn = true;
            showConnect = true;
            connectCounter = 3.0f;
        }
        else if (payload->type == Win_Lose) {
            BoolPacket* losePacket = (BoolPacket*)payload;

            if (!losePacket->flag)
                failure = true;
            else
                success = true;
        }
        else if (payload->type == Restart_Game) {
            BoolPacket* restartPacket = (BoolPacket*)payload;

            restartFlag = restartPacket->flag;
        }
    }
    else if (thisServer) {
        if (payload->type == Full_State) {
            ClientPacket* packet = (ClientPacket*)payload;
            switch (packet->playerNum) {
            case 2:
                player2->GetNetworkObject()->ReadClientPacket(*packet);
                break;
            case 3:
                player3->GetNetworkObject()->ReadClientPacket(*packet);
                break;
            case 4:
                player4->GetNetworkObject()->ReadClientPacket(*packet);
                break;
            }
        }
        else if (payload->type == Received_State) {
            ReceivedPacket* packet = (ReceivedPacket*)payload;

            clientReceived = packet->received;
        }
        else if (payload->type == Spawn_Player) {
            SpawnPacket* packet = (SpawnPacket*)payload;

            spawnFlag = packet->flag;
            spawnNum = packet->num;
        }
    }
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
    if (thisServer) { //detected a collision between players!
        MessagePacket newPacket;
        newPacket.messageID = COLLISION_MSG;
        newPacket.playerID = a->GetPlayerNum();

        thisClient->SendPacket(newPacket);

        newPacket.playerID = b->GetPlayerNum();
        thisClient->SendPacket(newPacket);
    }
}

void NetworkedGame::UpdateChooseServer(float dt) {
    audio->Update();
    renderer->Update(dt);
    renderer->GetUI()->Update(dt); //UI
    renderer->Render();
}

void NetworkedGame::DrawPad() {
    if (usePad) {
        if (localPlayer->GetSlot() == 0 || localPlayer->GetSlot() == 7) {
            Vector3 position = localPlayer->GetTransform().GetPosition();
            Vector3 p = localPlayer->FindGrid(Vector3(position.x, 4.5f, position.z));
            int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
            pad->GetTransform().SetPosition(p);
            int type = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type;
            if (localPlayer->GetSlot() == 0) {
                if (type >= 10000)
                    pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
                else
                    pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
            }
            else if (localPlayer->GetSlot() == 7) {
                if (!localPlayer->CanPlaceRail())
                    pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
                else
                    pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
            }
        }
        else {
            Vector3 position = localPlayer->GetTransform().GetPosition();
            position = Vector3(position.x, 5, position.z) - localPlayer->GetFace() * 5.0f;
            Vector3 p = localPlayer->FindGrid(Vector3(position.x, 4.5f, position.z));
            int index = p.x / 10 + (p.z / 10) * TutorialGame::GetGame()->GetNavigationGrid()->GetGridWidth();
            pad->GetTransform().SetPosition(position);
            int type = TutorialGame::GetGame()->GetNavigationGrid()->GetGridNode(index).type;
            if (type >= 10000)
                pad->GetRenderObject()->SetColour(Vector4(1, 0, 0, 0.4f));
            else
                pad->GetRenderObject()->SetColour(Vector4(1, 1, 0, 0.4f));
        }
    }
    else {
        pad->GetRenderObject()->SetColour(Vector4());
        pad->GetTransform().SetPosition(Vector3());
    }
}