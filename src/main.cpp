#include <thread>
#include "modloader/shared/modloader.hpp"
#include "GorillaLocomotion/Player.hpp"
#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp"
#include "beatsaber-hook/shared/utils/utils-functions.h"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "beatsaber-hook/shared/utils/utils.h"
#include "beatsaber-hook/shared/utils/il2cpp-utils-methods.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"
#include "GlobalNamespace/OVRInput.hpp"
#include "GlobalNamespace/OVRInput_Button.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Rigidbody.hpp"
#include "UnityEngine/SphereCollider.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Object.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/RaycastHit.hpp"
#include "UnityEngine/Physics.hpp"
#include "monkecomputer/shared/GorillaUI.hpp"
#include "monkecomputer/shared/Register.hpp"
#include "gorilla-utils/shared/GorillaUtils.hpp"
#include "gorilla-utils/shared/Callbacks/MatchMakingCallbacks.hpp"
#include "gorilla-utils/shared/Utils/Player.hpp"
#include "config.hpp"
#include "main.hpp"
#include "AntiTagFreezeWatchView.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

using namespace UnityEngine;
using namespace UnityEngine::XR;
using namespace GorillaLocomotion;
using namespace GlobalNamespace;

// Loads the config from disk using our modInfo, then returns it for use

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

bool inRoom = false;
bool TriggerLock = false;
GameObject* pointer;
Transform* rightHandT;

#include "GlobalNamespace/GorillaTagManager.hpp"

MAKE_HOOK_MATCH(GorillaTagManager_Update, &GlobalNamespace::GorillaTagManager::Update, void, GlobalNamespace::GorillaTagManager* self) {
    GorillaTagManager_Update(self);
    if (inRoom && config.enabled) {
    RaycastHit hitInfo;
    int layermask = 0b1 << 9;
    Player* playerInstance = Player::get_Instance();
    Rigidbody* playerPhysics = playerInstance->playerRigidBody;
    if(playerPhysics == nullptr) return;
    GameObject* playerGameObject = playerPhysics->get_gameObject();
    if(playerGameObject == nullptr) return;
    auto* player = playerGameObject->GetComponent<GorillaLocomotion::Player*>();
    //UnityEngine::Transform* transform = playerGameObject->get_transform();
    if (inRoom && config.enabled)
    {
        if (player->disableMovement == true)
        {
            player->disableMovement = false;
        }
    }
    }
}
MAKE_HOOK_MATCH(Player_Awake, &GorillaLocomotion::Player::Awake, void, GorillaLocomotion::Player* self) {
    Player_Awake(self);
    GorillaUtils::MatchMakingCallbacks::onJoinedRoomEvent() += {[&]() {
        Il2CppObject* currentRoom = CRASH_UNLESS(il2cpp_utils::RunMethod("Photon.Pun", "PhotonNetwork", "get_CurrentRoom"));

        if (currentRoom)
        {
            inRoom = !CRASH_UNLESS(il2cpp_utils::RunMethod<bool>(currentRoom, "get_IsVisible"));
        } else inRoom = true;
    }
    };
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    GorillaUI::Init();
    il2cpp_functions::Init();
    INSTALL_HOOK(getLogger(), Player_Awake);
    INSTALL_HOOK(getLogger(), GorillaTagManager_Update);
    GorillaUI::Register::RegisterWatchView<AntiTagFreeze::AntiTagFreezeWatchView*>("<b><i><color=#660e0e>Anti Tag Freeze</color></i></b>", VERSION);
    getLogger().info("Installing hooks...");
    // Install our hooks (none defined yet)
    LoadConfig();
    getLogger().info("Installed all hooks!");
}