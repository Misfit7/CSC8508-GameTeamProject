#include "NetworkObject.h"
#include "RenderObject.h"
#include "AnimationObject.h"
#include "./enet/enet.h"
using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(GameObject& o, int id) : object(o)	{ 
	deltaErrors = 0;
	fullErrors  = 0;
	networkID   = id;
    lastFullState.stateID = 0;
}

NetworkObject::~NetworkObject()	{
}

bool NetworkObject::ReadPacket(GamePacket &p) {
    //if (p.type == Delta_State) {
    //    return ReadDeltaPacket((DeltaPacket &)p);
    //}
    if (p.type == Full_State) {
        return ReadFullPacket((FullPacket &)p);
    }
    return false; // this isn’t a packet we care about!
}

bool NetworkObject::ReadClientPacket(ClientPacket& p) {
    object.SetButton(0, p.buttonstates[0]);
    object.SetButton(1, p.buttonstates[1]);
    object.SetButton(2, p.buttonstates[2]);
    object.SetButton(3, p.buttonstates[3]);
    object.SetButton(4, p.buttonstates[4]);
    object.SetButton(5, p.buttonstates[5]);
    object.SetButton(6, p.buttonstates[6]);
    object.SetButton(7, p.buttonstates[7]);
    object.SetButton(8, p.buttonstates[8]);
    return true;
}

bool NetworkObject::WritePacket(GamePacket** p, bool deltaFrame, int stateID) {
	//if (deltaFrame) {
	//	if (!WriteDeltaPacket(p, stateID)) {
	//		return WriteFullPacket(p);
	//	}
	//}
	return WriteFullPacket(p);
}
//Client objects recieve these packets
bool NetworkObject::ReadDeltaPacket(DeltaPacket& p) {
    if (p.fullID != lastFullState.stateID) {
        return false; // can’t delta this frame
    }
    UpdateStateHistory(p.fullID);
    if (p.objectID == object.GetNetworkObject()->GetNetworkID()) {
        Vector3 fullPos = lastFullState.position;
        Quaternion fullOrientation = lastFullState.orientation;

        fullPos.x += p.pos[0];
        fullPos.y += p.pos[1];
        fullPos.z += p.pos[2];

        fullOrientation.x += ((float)p.orientation[0]) / 127.0f;
        fullOrientation.y += ((float)p.orientation[1]) / 127.0f;
        fullOrientation.z += ((float)p.orientation[2]) / 127.0f;
        fullOrientation.w += ((float)p.orientation[3]) / 127.0f;

        object.GetTransform().SetPosition(fullPos);
        object.GetTransform().SetOrientation(fullOrientation);

        if (networkID <= 2) {
            int fullCurrentFrame = lastFullState.currentFrame;

            fullCurrentFrame += p.currentFrame;

            object.GetRenderObject()->GetAnimationObject()->SetCurrentFrame(fullCurrentFrame);
        }
        else if (object.GetTypeID() == 10010 || object.GetTypeID() == 10086) {
            Vector3 fullScale = lastFullState.scale;

            fullScale.x += p.scale[0];
            fullScale.y += p.scale[1];
            fullScale.z += p.scale[2];

            object.GetTransform().SetScale(fullScale);
        }
        else if (object.GetTypeID() == 7 || object.GetTypeID() == 23) {
            Vector4 fullColour = lastFullState.colour;

            fullColour.x += ((float)p.colour[0]) / 127.0f;
            fullColour.y += ((float)p.colour[1]) / 127.0f;
            fullColour.z += ((float)p.colour[2]) / 127.0f;
            fullColour.w += ((float)p.colour[3]) / 127.0f;

            object.GetRenderObject()->SetColour(fullColour);
        }
    }
    return true;
}

bool NetworkObject::ReadFullPacket(FullPacket& p) {
    if (p.fullState.stateID < lastFullState.stateID) {
        //std::cout << "FullState: " << p.fullState.stateID << std::endl;
        //std::cout << "LastFullState: " << lastFullState.stateID << std::endl;
        //std::cout << "WorldID: " << object.GetWorldID() << std::endl;
        //std::cout << "Client: Received an 'old' packet, ignore!" << std::endl;
        return false;
    }
    if (p.objectID == object.GetNetworkObject()->GetNetworkID()) {
        lastFullState = p.fullState;
        if (!(object.GetTypeID() == 10010) && !(object.GetTypeID() == 10086)) {
            //std::cout << "Type: " << object.GetTypeID() << std::endl;
            object.GetTransform().SetPosition(lastFullState.position);
            object.GetTransform().SetOrientation(lastFullState.orientation);

            if (object.GetTypeID() == 1) {
                if(object.GetRenderObject()->GetAnimationObject())object.GetRenderObject()->GetAnimationObject()->SetCurrentFrame(lastFullState.currentFrame);
                object.SetSlot(lastFullState.slot);
                object.SetSlotNum(lastFullState.slotNum);
            }
            else if (object.GetTypeID() == 22) {
                object.GetRenderObject()->SetColour(lastFullState.colour);
            }
            else if (object.GetTypeID() == 5 || object.GetTypeID() == 6 || object.GetTypeID() == 7) {
                object.GetTransform().SetScale(lastFullState.scale);
            }
            else if (object.GetTypeID() == 20) {
                object.SetFlag1(lastFullState.flag1);
                object.SetFloat1(lastFullState.float1);
                object.SetFloat2(lastFullState.float2);
                object.SetFloat3(lastFullState.float3);
                object.SetPosition1(lastFullState.position1);
            }
            else if (object.GetTypeID() == 23) {
                object.GetRenderObject()->SetColour(lastFullState.colour);
                object.SetFloat1(lastFullState.float1);
            }
            else if (object.GetTypeID() == 10 || object.GetTypeID() == 11) {
                object.GetRenderObject()->GetAnimationObject()->SetCurrentFrame(lastFullState.currentFrame);
            }
            else if (object.GetTypeID() == 4) {
                object.GetRenderObject()->SetColour(lastFullState.colour);
                object.SetFlag1(lastFullState.flag1);
            }
        }
        else {
            object.GetTransform().SetScale(lastFullState.scale);
        }
    }

    stateHistory.emplace_back(lastFullState);

    return true;
}

bool NetworkObject::WriteDeltaPacket(GamePacket** p, int stateID) {
    DeltaPacket* dp = new DeltaPacket();
    NetworkState state;
    if (!GetNetworkState(stateID, state)) {
        return false; // can’t delta!
    }

    dp->fullID = stateID;
    dp->objectID = networkID;

    Vector3 currentPos = object.GetTransform().GetPosition();
    Quaternion currentOrientation = object.GetTransform().GetOrientation();

    currentPos -= state.position;
    currentOrientation -= state.orientation;

    dp->pos[0] = (char)currentPos.x;
    dp->pos[1] = (char)currentPos.y;
    dp->pos[2] = (char)currentPos.z;

    dp->orientation[0] = (char)(currentOrientation.x * 127.0f);
    dp->orientation[1] = (char)(currentOrientation.y * 127.0f);
    dp->orientation[2] = (char)(currentOrientation.z * 127.0f);
    dp->orientation[3] = (char)(currentOrientation.w * 127.0f);

    if (networkID <= 2) {
        int currentFrame = object.GetRenderObject()->GetAnimationObject()->GetCurrentFrame();

        currentFrame -= state.currentFrame;

        dp->currentFrame = currentFrame;
    }
    else if (object.GetTypeID() == 10010 || object.GetTypeID() == 10086) {
        Vector3 currentScale = object.GetTransform().GetScale();

        currentScale -= state.scale;

        dp->scale[0] = (char)currentScale.x;
        dp->scale[1] = (char)currentScale.y;
        dp->scale[2] = (char)currentScale.z;
    }
    else if (object.GetTypeID() == 7 || object.GetTypeID() == 23) {
        Vector4 currentColour = object.GetRenderObject()->GetColour();

        currentColour -= state.colour;

        dp->colour[0] = (char)(currentColour.x * 127.0f);
        dp->colour[1] = (char)(currentColour.y * 127.0f);
        dp->colour[2] = (char)(currentColour.z * 127.0f);
        dp->colour[3] = (char)(currentColour.w * 127.0f);
    }

    *p = dp;
    return true;
}

bool NetworkObject::WriteFullPacket(GamePacket** p) {
    FullPacket* fp = new FullPacket();
    fp->objectID = networkID;
    fp->fullState.stateID = lastFullState.stateID++;

    if (!(object.GetTypeID() == 10010) && !(object.GetTypeID() == 10086)) {
        //std::cout << "Server: " << networkID <<std::endl;
        fp->fullState.position = object.GetTransform().GetPosition();
        fp->fullState.orientation = object.GetTransform().GetOrientation();

        if (object.GetTypeID() == 1) {
            if(object.GetRenderObject()->GetAnimationObject()) fp->fullState.currentFrame = object.GetRenderObject()->GetAnimationObject()->GetCurrentFrame();
            fp->fullState.slot = object.GetSlot();
            fp->fullState.slotNum = object.GetSlotNum();
        }
        else if (object.GetTypeID() == 22) {
            fp->fullState.colour = object.GetRenderObject()->GetColour();
        }
        else if (object.GetTypeID() == 5 || object.GetTypeID() == 6 || object.GetTypeID() == 7) {
            fp->fullState.scale = object.GetTransform().GetScale();
        }
        else if (object.GetTypeID() == 20) {
            fp->fullState.flag1 = object.GetFlag1();
            fp->fullState.float1 = object.GetFloat1();
            fp->fullState.float2 = object.GetFloat2();
            fp->fullState.float3 = object.GetFloat3();
            fp->fullState.position1 = object.GetPosition1();
        }
        else if (object.GetTypeID() == 23) {
            fp->fullState.colour = object.GetRenderObject()->GetColour();
            fp->fullState.float1 = object.GetFloat1();
        }
        else if (object.GetTypeID() == 10 || object.GetTypeID() == 11) {
            fp->fullState.currentFrame = object.GetRenderObject()->GetAnimationObject()->GetCurrentFrame();
        }
        else if (object.GetTypeID() == 4) {
            fp->fullState.colour = object.GetRenderObject()->GetColour();
            fp->fullState.flag1 = object.GetFlag1();
        }

        *p = fp;
        return true;
    }
  
    else {
        if (object.GetFlag1()) {
            fp->fullState.scale = object.GetTransform().GetScale();
            *p = fp;
            return true;
        }
        else return false;
    }
}

void NetworkObject::UpdateStateHistory(int minID) {
    for (auto i = stateHistory.begin(); i < stateHistory.end();) {
        if ((*i).stateID < minID) {
            i = stateHistory.erase(i);
        }
        else {
            ++i;
        }
    }
}

NetworkState& NetworkObject::GetLatestNetworkState() {
    return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
    for (auto i = stateHistory.begin(); i < stateHistory.end(); ++i) {
        if ((*i).stateID == stateID) {
            state = (*i);
            return true;
        }
    }
    return false;
}