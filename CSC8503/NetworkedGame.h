#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"

namespace NCL {
    namespace CSC8503 {
        class GameServer;
        class GameClient;
        class NetworkPlayer;

        class NetworkedGame : public TutorialGame, public PacketReceiver {
        public:
            NetworkedGame();
            ~NetworkedGame();

            void StartAsServer();
            void StartAsClient(char a, char b, char c, char d);

            void UpdateGame(float dt) override;

            void UpdateNetworkedPlaying(float dt);
            void UpdatePaused(float dt) override;
            void UpdateFailure(float dt) override;
            void UpdateFinish(float dt) override;

            void UpdateKeys() override;

            void SpawnPlayer();

            void SpawnCarriage();

            void SynchronizeGameObjects();

            void UpdateGameObjects();

            void SpawnGameObjects();

            void StartLevel();

            void ReceivePacket(int type, GamePacket* payload, int source) override;

            void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

            bool IsServer() {
                return thisServer != nullptr;
            }
            bool IsClient() {
                return thisClient != nullptr;
            }

            PlayerObject* GetLocalPlayer() const {
                return localPlayer;
            }

            int GetPlayerNum() const {
                return playerNum;
            }

            bool DisplayPlayerUI() const {
                return playerUI;
            }

            bool GetCutTreeFlag() const {
                return treeCut;
            }
            void SetCutTreeFlag(bool f) {
                treeCut = f;
            }
            bool GetDigRockFlag() const {
                return rockDug;
            }
            void SetDigRockFlag(bool f) {
                rockDug = f;
            }
            bool GetUpdateMaterialFlag() const {
                return materialUpdating;
            }
            void SetUpdateMaterialFlag(bool f) {
                materialUpdating = f;
            }
            bool GetProduceRailFlag() const {
                return railProduced;
            }
            void SetProduceRailFlag(bool f) {
                railProduced = f;
            }
            bool GetUpdateRailFlag() const {
                return updateRail;
            }
            void SetUpdateRailFlag(bool f) {
                updateRail = f;
            }
            bool GetBuildBridgeFlag() {
                return bridgeBuilt;
            }
            void SetBuildBridgeFlag(bool f) {
                bridgeBuilt = f;
            }
            bool GetRemovePlayerFlag() const {
                return playerRemoved;
            }
            void SetRemovedPlayerFlag(bool f) {
                playerRemoved = f;
            }

            void SetTreeCutTag(int t) {
                treeCutTag = t;
            }
            void SetPlankNetworkID(int id) {
                plankNetworkID = id;
            }
            void SetTreeWorldID(int id) {
                treeWorldID = id;
            }

            void SetRockDugTag(int t) {
                rockDugTag = t;
            }
            void SetStoneNetworkID(int id) {
                stoneNetworkID = id;
            }
            void SetRockWorldID(int id) {
                rockWorldID = id;
            }

            void SetMaterialUpdatingTag(int t) {
                materialUpdatingTag = t;
            }
            void SetRemovedPlankNetworkID(int id) {
                removedPlankNetworkID = id;
            }
            void SetRemovedStoneNetworkID(int id) {
                removedStoneNetworkID = id;
            }

            void SetRailProducedTag(int t) {
                railProducedTag = t;
            }
            void SetRailNetworkID(int id) {
                railNetworkID = id;
            }

            void SetUpdateRailTag(int t) {
                updateRailTag = t;
            }
            void SetUpdateRailNetworkID(int id) {
                updateRailNetworkID = id;
            }

            void SetBridgeBuiltTag(int t) {
                bridgeBuiltTag = t;
            }
            void SetWaterWorldID(int id) {
                waterWorldID = id;
            }

            void SetPlayerRemovedTag(int t) {
                removePlayerTag = t;
            }
            void SetRemovedPlayerNetworkID(int id) {
                removedPlayerNetworkID = id;
            }

            float SetConnectCounter(float c) {
                connectCounter = c;
            }
            float SetDisConnectCounter(float c) {
                disconnectCounter = c;
            }

            static NetworkedGame* GetNetworkedGame() {
                return networkInstance;
            };

        protected:
            void UpdateAsServer(float dt);
            void UpdateAsClient(float dt);

            void ServerBroadcast();

            void BroadcastSnapshot(bool deltaFrame);
            void ClientSend(bool deltaFrame);
            void UpdateMinimumState();

            void UpdateChooseServer(float dt);

            void DrawPad();

            std::map<int, int> stateIDs;

            GameServer* thisServer;
            GameClient* thisClient;
            //ClientPacket clientPacket;
            float servertimeToNextPacket;
            float clienttimeToNextPacket;
            int packetsToSnapshot;

            std::vector<NetworkObject*> networkObjects;

            std::map<int, GameObject*> serverPlayers;
            PlayerObject* localPlayer;
            int playerNum;
            int spawnNum;

            static NetworkedGame* networkInstance;
            
            bool clientReceived = false;

            bool playerUI = true;

            bool spawn = false;
            bool objectSpawned = false;

            bool treeCut = false;
            bool rockDug = false;
            bool materialUpdating = false;
            bool railProduced = false;
            bool updateRail = false;
            bool bridgeBuilt = false;
            bool playerRemoved = false;

            int treeCutTag = 0;
            int plankNetworkID;
            int treeWorldID;

            int rockDugTag = 0;
            int stoneNetworkID;
            int rockWorldID;

            int materialUpdatingTag = 0;
            int removedPlankNetworkID;
            int removedStoneNetworkID;

            int railProducedTag = 0;
            int railNetworkID;

            int updateRailTag = 0;
            int updateRailNetworkID;

            int newObjectTag = 0;
            vector<int> objectType;
            vector<int> objectNetworkID;

            int bridgeBuiltTag = 0;
            int waterWorldID;

            int removePlayerTag = 0;
            int removedPlayerNetworkID = 0;

            int removeObjectTag = 0;
            vector<int> removedObjectNetworkID;

            bool showConnect;
            bool showDisconnect;

            float connectCounter = 0.0f;
            float disconnectCounter = 0.0f;
        };
    }
}

