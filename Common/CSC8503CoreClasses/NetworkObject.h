#pragma once
#include "GameObject.h"
#include "NetworkBase.h"
#include "NetworkState.h"

namespace NCL::CSC8503 {
    class GameObject;

    struct FullPacket : public GamePacket {
        int		objectID = -1;
        NetworkState fullState;

        FullPacket() {
            type = Full_State;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    struct DeltaPacket : public GamePacket {
        int		fullID = -1;
        int		objectID = -1;
        char	pos[3];
        char	orientation[4];
        char    scale[3];
        char    colour[4];
        int     currentFrame;

        DeltaPacket() {
            type = Delta_State;
            size = sizeof(DeltaPacket) - sizeof(GamePacket);
        }
    };

    struct UpdatePacket : public GamePacket {
        Vector3 position;
        int     networkID1;
        int     networkID2;
        int     worldID;
        int     typeID;
        int     tag;

        UpdatePacket() {
            type = Update_Objects;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    struct NumPacket :public GamePacket {
        int num;

        NumPacket() {
            type = Client_Num;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    struct BoolPacket :public GamePacket {
        bool flag;

        BoolPacket() {
            type = Default_Bool;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    struct SpawnPacket :public GamePacket {
        bool flag;
        int num;

        SpawnPacket() {
            type = Spawn_Player;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    struct ClientPacket : public GamePacket {
        int		lastID;
        int     playerNum;
        char	buttonstates[9] = { 0 };

        ClientPacket() {
            size = sizeof(ClientPacket);
            type = BasicNetworkMessages::Full_State;
        }
    };

    struct ReceivedPacket :public GamePacket {
        bool received;

        ReceivedPacket() {
            type = Received_State;
            size = sizeof(FullPacket) - sizeof(GamePacket);
        }
    };

    class NetworkObject {
    public:
        NetworkObject(GameObject& o, int id);
        virtual ~NetworkObject();

        //Called by clients
        virtual bool ReadPacket(GamePacket& p);
        //Called by servers
        virtual bool WritePacket(GamePacket** p, bool deltaFrame, int stateID);
        virtual bool ReadClientPacket(ClientPacket& p);

        void UpdateStateHistory(int minID);

        int GetNetworkID() const {
            return networkID;
        }

        GameObject& GetGameObject() const {
            return object;
        }

        NetworkState GetLastFullState() const {
            return lastFullState;
        }

        bool IsUpdate() const {
            return update;
        }
        void SetUpdate(bool u) {
            update = u;
        }

    protected:

        NetworkState& GetLatestNetworkState();

        bool GetNetworkState(int frameID, NetworkState& state);

        virtual bool ReadDeltaPacket(DeltaPacket& p);
        virtual bool ReadFullPacket(FullPacket& p);

        virtual bool WriteDeltaPacket(GamePacket** p, int stateID);
        virtual bool WriteFullPacket(GamePacket** p);

        GameObject& object;

        NetworkState lastFullState;

        std::vector<NetworkState> stateHistory;

        int deltaErrors;
        int fullErrors;

        bool update = true;
        int networkID;
    };
}