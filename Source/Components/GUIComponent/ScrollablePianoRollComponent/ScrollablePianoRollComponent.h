/*
  ==============================================================================

    ScrollablePianoRollComponent.h
    Created: 22 Feb 2020 6:06:34pm
    Author:  Jiawen Huang

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <functional>

#include "../../Globals.h"

#include "PianoRollNote.h"
#include "NoteLayer.h"
#include "CustomKeyboardComponent.h"

class OtherLookAndFeel;
class DragTextButton;
class RectNote;
class PianoRollListBox;

//==============================================================================
class OtherLookAndFeel : public LookAndFeel_V4
{
public:
    OtherLookAndFeel();

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                               bool, bool isButtonDown) override;
};

//==============================================================================
class PianoRollComponent   : public Component
{
public:
    PianoRollComponent();
    
    ~PianoRollComponent() override;
    
    void init(std::shared_ptr<PlayerComponent> pPlayer, int numTimeStampsForPianoRoll);
    
    void setSyncFunctionPointer();

    void paint (Graphics& g) override;
    
    void resized() override;
    
    //==============================================================================
    
    void addNote(PianoRollNote *newNote);
    
    //==============================================================================
    void setPreview(bool ifPreview);

    int getViewPositionX();
    
    int getBoxWidth();
    
    int getBoxHeight();
    
    int getCanvasWidth();
    
    void setViewPositionX(int viewPositionX)
    {
        m_pNoteLayer->setViewPositionX(viewPositionX);
    }
    
    // This little function avoids a bit of code-duplication by adding a component to
    // our list as well as calling addAndMakeVisible on it..
    template <typename ComponentType>
    ComponentType* addToList (ComponentType* newComp);
    
    std::function<void(int)> m_syncScrollBars = nullptr;
    
private:
    OwnedArray<Component> components;
    
    OtherLookAndFeel otherLookAndFeel;
    
    CustomKeyboardComponent         *m_pKeyboardComponent;
    
    NoteLayer                       *m_pNoteLayer;
    
    bool                            m_Preview = true;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PianoRollComponent)
};

class ScrollablePianoRollComponent: public Component
{
public:

    explicit ScrollablePianoRollComponent(std::shared_ptr<PlayerComponent> pPlayer, int numTimeStampsForPianoRoll = 30);

    void setSyncFunctionPointer();
    
    void resized() override;
    
    void addNote(PianoRollNote *newNote);
    
    void setPreview(bool ifPreview);
    
    int getViewPositionX();
    
    int getBoxWidth();
    
    int getBoxHeight();
    
    int getCanvasWidth();

    void setViewPositionX(int viewPositionX)
    {
        m_Cpn.setViewPositionX(viewPositionX);
    }
    
    std::function<void(int)> m_syncScrollBars = nullptr;
    
private:

    Viewport m_ViewPort;
    PianoRollComponent m_Cpn;
    bool m_bPreview = true;
};
