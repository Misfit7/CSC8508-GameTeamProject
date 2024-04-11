#pragma once
#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld &g);

			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);

			virtual void UpdateServer();

			bool GetSpawnFlag() const {
				return spawnFlag;
			}
			void SetSpawnFlag(bool f) {
				spawnFlag = f;
			}

			bool IsConnected() const {
				return connected;
			}

			int GetClientCount() const {
				return clientCount;
			}

			void SetRemovePlayer(int r) {
				removePlayer = r;
			}
			int GetRemovePlayer() const {
				return removePlayer;
			}

			bool ConnectDebug() const {
				return connectDebug;
			}
			void SetConnectDebug(bool d) {
				connectDebug = d;
			}
			bool DisconnectDebug() const {
				return disconnectDebug;
			}
			void SetDisconnectDebug(bool d) {
				disconnectDebug = d;
			}

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld*	gameWorld;

			int incomingDataRate;
			int outgoingDataRate;

			int removePlayer = 0;

			bool spawnFlag = false;
			bool connected = false;
			bool connectDebug = false;
			bool disconnectDebug = false;
		};
	}
}
