/*
  ==============================================================================

    MenuComponent.h
    Created: 12 Jan 2020 12:28:18pm
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Globals.h"
#include <functional>

//==============================================================================
/*
*/

class MenuComponent    : public Component,
                         public ApplicationCommandTarget,
                         public MenuBarModel
{
public:
    MenuComponent();
    ~MenuComponent();
    
    enum CommandIDs {
        fileOpen = 1,
        fileExportAudio,
//        fileExportMIDI,
        editUndo,
        editRedo,
        editCut,
        editCopy,
        editPaste,
        helpDocumentation
    };
    
    typedef std::function<bool(CommandIDs)> cbfunc;
    
    void paint (Graphics&) override;
    void resized() override;
    StringArray getMenuBarNames() override;
    PopupMenu getMenuForIndex(int topLevelMenuIndex, const String &menuName) override;
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands (Array<CommandID>& c) override;
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override;
    void menuItemSelected (int /*menuItemID*/, int /*topLevelMenuIndex*/) override;
    bool perform (const InvocationInfo& info) override;
    
    void setCallback(cbfunc func);

private:
    ApplicationCommandManager commandManager;
    std::unique_ptr<MenuBarComponent> menuBar;
    
    cbfunc callbackFunc = 0;
    
//    class EditCommandTarget    : public Component,
//                                  public ApplicationCommandTarget
//    {
//    public:
//        EditCommandTarget (ApplicationCommandManager& m)
//            : commandManager (m),
//              helpCommandTarget (commandManager)
//        {
//            commandManager.registerAllCommandsForTarget (this);
//
//            addAndMakeVisible (helpCommandTarget);
//        }
//
//        void resized() override
//        {
////            helpCommandTarget.setBounds (getLocalBounds().reduced (50));
//        }
//
//        void paint (Graphics& g) override {}
//
//        //==============================================================================
//        ApplicationCommandTarget* getNextCommandTarget() override
//        {
//            return &helpCommandTarget;
//        }
//
//        void getAllCommands (Array<CommandID>& c) override
//        {
//            Array<CommandID> commands { editUndo, editRedo, editCut, editCopy, editPaste };
//            c.addArray (commands);
//        }
//
//        void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override
//        {
//            switch (commandID) {
//                case editUndo:
//                    result.setInfo("Undo", "Undo's last command", "Edit", 0);
//                    result.addDefaultKeypress('z', ModifierKeys::commandModifier);
//                    break;
//
//                case editRedo:
//                    result.setInfo("Redo", "Redo's last command", "Edit", 0);
//                    result.addDefaultKeypress('y', ModifierKeys::commandModifier);
//                    break;
//
//                case editCut:
//                    result.setInfo("Cut", "Cut selection", "Edit", 0);
//                    result.addDefaultKeypress('x', ModifierKeys::commandModifier);
//                    break;
//
//                case editCopy:
//                    result.setInfo("Copy", "Copy selection", "Edit", 0);
//                    result.addDefaultKeypress('c', ModifierKeys::commandModifier);
//                    break;
//
//                case editPaste:
//                    result.setInfo("Paste", "Paste at position", "Edit", 0);
//                    result.addDefaultKeypress('v', ModifierKeys::commandModifier);
//                    break;
//
//                default:
//                    break;
//            }
//        }
//
//        bool perform (const InvocationInfo& info) override
//        {
//            switch (info.commandID) {
//                case editUndo:
//                    std::cout << "Undo" << std::endl;
//                    break;
//
//                case editRedo:
//                    std::cout << "Redo" << std::endl;
//                    break;
//
//                case editCut:
//                    std::cout << "Cut" << std::endl;
//                    break;
//
//                case editCopy:
//                    std::cout << "Copy" << std::endl;
//                    break;
//
//                case editPaste:
//                    std::cout << "Paste" << std::endl;
//                    break;
//
//                default:
//                    break;
//            }
//            return true;
//        }
//
//    private:
//
//        ApplicationCommandManager& commandManager;
//
//    };

    struct HelpCommandTarget    : public Component,
                                  public ApplicationCommandTarget
    {
        HelpCommandTarget (ApplicationCommandManager& m)
                : commandManager (m)
        {
            commandManager.registerAllCommandsForTarget (this);
        }

        void paint (Graphics& g) override {}

        //==============================================================================
        ApplicationCommandTarget* getNextCommandTarget() override
        {
            // this will return the next parent component that is an ApplicationCommandTarget
            return findFirstTargetParentComponent();
        }

        void getAllCommands (Array<CommandID>& c) override
        {
            Array<CommandID> commands { helpDocumentation };

            c.addArray (commands);
        }

        void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result) override
        {
            switch (commandID)
            {
                case helpDocumentation:
                    result.setInfo ("Documentation", "Open documentation", "Help", 0);
                    result.addDefaultKeypress ('h', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
                    break;

                default:
                    break;
            }
        }

        bool perform (const InvocationInfo& info) override
        {
            switch (info.commandID)
            {
                case helpDocumentation:
                    std::cout << "documentation" << std::endl;
                    break;

                default:
                    return false;
            }
            return true;
        }

        ApplicationCommandManager& commandManager;
    };
    HelpCommandTarget helpCommandTarget {commandManager};
//    EditCommandTarget editCommandTarget {commandManager};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MenuComponent)
};
