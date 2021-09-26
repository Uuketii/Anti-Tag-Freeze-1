#include "AntiTagFreezeWatchView.hpp"
#include "config.hpp"
#include "monkecomputer/shared/ViewLib/MonkeWatch.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"

DEFINE_TYPE(AntiTagFreeze, AntiTagFreezeWatchView);

using namespace GorillaUI;
using namespace UnityEngine;

namespace AntiTagFreeze
{
    void AntiTagFreezeWatchView::Awake()
    {
        settingSelector = new UISelectionHandler(EKeyboardKey::Up, EKeyboardKey::Down, EKeyboardKey::Enter, true, false);
        ctrlSelector = new UISelectionHandler(EKeyboardKey::Left, EKeyboardKey::Right, EKeyboardKey::Enter, false, true);

        settingSelector->max = 2;
        ctrlSelector->max = 2;
    }

    void AntiTagFreezeWatchView::DidActivate(bool firstActivation)
    {
        std::function<void(bool)> fun = std::bind(&AntiTagFreezeWatchView::OnEnter, this, std::placeholders::_1);
        settingSelector->selectionCallback = fun;
        Redraw();
    }

    void AntiTagFreezeWatchView::OnEnter(int index)
    {
        if (settingSelector->currentSelectionIndex == 0) 
        {
            config.enabled ^= 1;
            SaveConfig();
        }
    }

    void AntiTagFreezeWatchView::Redraw()
    {
        text = "";

        DrawHeader();
        DrawBody();

        MonkeWatch::Redraw();
    }

    void AntiTagFreezeWatchView::DrawHeader()
    {
        text += "<size=80><b><color=#660e0e>Anti Tag Freeze</color></b>\n</size>";
    }

    void AntiTagFreezeWatchView::DrawBody()
    {
        text += "<size=60>\n</size>";
        text += settingSelector->currentSelectionIndex == 1 ? "<size=60><color=#FF5115>></color></size>" : "<size=60> </size>";
        text += "<size=60> Made by <color=#9403fc>Uuketi</color></size>";

        if (config.enabled)
        {
            text += "<size=60><color=#00ff00>\nEnabled on a budget.</color></size>";
        }
    }

    void AntiTagFreezeWatchView::OnKeyPressed(int value)
    {
        EKeyboardKey key = (EKeyboardKey)value;
        if (!settingSelector->HandleKey(key)) // if it was not up/down/enter
        {
            switch (settingSelector->currentSelectionIndex)
            {
                case 0:
                    break;
                case 1:
                    ctrlSelector->HandleKey(key);
                    break;
                default:
                    break;
            }
            config.ctrl = ctrlSelector->currentSelectionIndex;
            SaveConfig();
        }
        Redraw();
    }
}