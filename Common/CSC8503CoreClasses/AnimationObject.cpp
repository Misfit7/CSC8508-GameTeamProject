#include "AnimationObject.h"

using namespace NCL::CSC8503;
using namespace NCL;

AnimationObject::AnimationObject() {
	anim1       = nullptr;
	anim2       = nullptr;
	anim3       = nullptr;
	anim4       = nullptr;
	anim5       = nullptr;
	activeAnim  = nullptr;
}

void AnimationObject::UpdateFlags(int num) {
	flag1 = num != 1 ? true : false;
	flag2 = num != 2 ? true : false;
	flag3 = num != 3 ? true : false;
	flag4 = num != 4 ? true : false;
	flag5 = num != 5 ? true : false;
	flag6 = num != 6 ? true : false;
	flag7 = num != 7 ? true : false;
	flag8 = num != 8 ? true : false;
}
	