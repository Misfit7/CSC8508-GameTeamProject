#pragma once

#include "MeshAnimation.h"

namespace NCL {
    using namespace NCL::Rendering;

    namespace CSC8503 {

        class AnimationObject
        {
        public:
            AnimationObject();
            ~AnimationObject() {};

            void SetAnim1(MeshAnimation* anim) {
                anim1 = anim;
            }
            MeshAnimation* GetAnim1() const {
                return anim1;
            }
            void SetAnim2(MeshAnimation* anim) {
                anim2 = anim;
            }
            MeshAnimation* GetAnim2() const {
                return anim2;
            }
            void SetAnim3(MeshAnimation* anim) {
                anim3 = anim;
            }
            MeshAnimation* GetAnim3() const {
                return anim3;
            }
            void SetAnim4(MeshAnimation* anim) {
                anim4 = anim;
            }
            MeshAnimation* GetAnim4() const {
                return anim4;
            }
            void SetAnim5(MeshAnimation* anim) {
                anim5 = anim;
            }
            MeshAnimation* GetAnim5() const {
                return anim5;
            }
            void SetAnim6(MeshAnimation* anim) {
                anim6 = anim;
            }
            MeshAnimation* GetAnim6() const {
                return anim6;
            }
            void SetAnim7(MeshAnimation* anim) {
                anim7 = anim;
            }
            MeshAnimation* GetAnim7() const {
                return anim7;
            }
            void SetAnim8(MeshAnimation* anim) {
                anim8 = anim;
            }
            MeshAnimation* GetAnim8() const {
                return anim8;
            }
            void SetActiveAnim(MeshAnimation* anim) {
                activeAnim = anim;
            }
            MeshAnimation* GetActiveAnim() const {
                return activeAnim;
            }

            void SetFlag1(bool f) {
                flag1 = f;
            }
            bool GetFlag1() const {
                return flag1;
            }
            void SetFlag2(bool f) {
                flag2 = f;
            }
            bool GetFlag2() const {
                return flag2;
            }
            void SetFlag3(bool f) {
                flag3 = f;
            }
            bool GetFlag3() const {
                return flag3;
            }
            void SetFlag4(bool f) {
                flag4 = f;
            }
            bool GetFlag4() const {
                return flag4;
            }
            void SetFlag5(bool f) {
                flag5 = f;
            }
            bool GetFlag5() const {
                return flag5;
            }
            void SetFlag6(bool f) {
                flag6 = f;
            }
            bool GetFlag6() const {
                return flag6;
            }
            void SetFlag7(bool f) {
                flag7 = f;
            }
            bool GetFlag7() const {
                return flag7;
            }
            void SetFlag8(bool f) {
                flag8 = f;
            }
            bool GetFlag8() const {
                return flag8;
            }

            void SetIdle(bool i) {
                idle = i;
            }
            bool HasIdle() const {
                return idle;
            }

            void UpdateFlags(int num);

            int GetCurrentFrame() const { return currentFrame; }
            void SetCurrentFrame(int f) { currentFrame = f; }

            float GetFrameTime() const { return frameTime; }
            void SetFrameTime(float f) { frameTime = f; }

        protected:
            MeshAnimation* anim1;
            MeshAnimation* anim2;
            MeshAnimation* anim3;
            MeshAnimation* anim4;
            MeshAnimation* anim5;
            MeshAnimation* anim6;
            MeshAnimation* anim7;
            MeshAnimation* anim8;
            MeshAnimation* activeAnim;
            bool           flag1 = true;
            bool           flag2 = true;
            bool           flag3 = true;
            bool           flag4 = true;
            bool           flag5 = true;
            bool           flag6 = true;
            bool           flag7 = true;
            bool           flag8 = true;
            bool           idle = true;
            float          currentFrame = 0;
            float          frameTime = 0.0f;
        };
    }
}