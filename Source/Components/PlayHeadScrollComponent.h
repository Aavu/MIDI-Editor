/*
  ==============================================================================

    PlayHeadScrollComponent.h
    Created: 20 Apr 2020 12:22:25am
    Author:  Raghavasimhan Sankaranarayanan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Globals.h"

class TimeAxisButton: public TextButton
{
public:
    
    void paint (Graphics& g) override
    {
        g.setColour (Colours::grey);
        
        // draw boxes
        for (int i = 0; i < m_iNumBox; i++)
            g.drawRect(1.F*i*m_iboxWidth, 0.F, 1.F*m_iboxWidth, 1.F*getHeight(), 0.5);
        
    }
    
    void setBoxWidthAndNumBox(int boxWidth, int numBox)
    {
        m_iboxWidth = boxWidth;
        m_iNumBox = numBox;
    }
    
    std::function<void(int posX)> childClicked = nullptr;
    
private:
    void mouseUp (const MouseEvent& event) override {
        if (!childClicked)
            return;
        
        childClicked(event.getPosition().getX());
    }
    
    void mouseDrag (const MouseEvent& event) override {
        if (!childClicked)
            return;
        
        childClicked(event.getPosition().getX());
    }
    
    int                 m_iAxisWidth;
    int                 m_iboxWidth = Globals::PianoRoll::initNoteWidth;
    int                 m_iNumBox = Globals::PianoRoll::initTimeStamps;
};

class PlayHeadScrollComponent : public Component {
public:
    PlayHeadScrollComponent()
    {
        //m_timeAxis.setColour(TextButton::buttonColourId, Colours::black);
        m_iAxisWidth = m_iboxWidth * m_iNumBox;
        m_TimeAxis.setSize(m_iAxisWidth, 40);
        
        m_ViewPort.setScrollBarsShown(false, false, true, true);
        m_ViewPort.setViewedComponent(&m_TimeAxis, false);
        addAndMakeVisible(&m_ViewPort);
    }

    void setBoxWidthAndNumBox(int boxWidth, int numBox)
    {
        m_iboxWidth = boxWidth;
        m_iNumBox = numBox;
        m_iAxisWidth = m_iboxWidth * m_iNumBox;
        m_TimeAxis.setBoxWidthAndNumBox(boxWidth, numBox);
    }

    void resized() override {
        m_ViewPort.setBoundsRelative(0.0f, 0.0f, 1.0f, 1.0f);
        m_TimeAxis.setSize(m_iAxisWidth, getHeight());
    }
    
    void setViewPositionX(int setViewPosition)
    {
        
    }

    TimeAxisButton      m_TimeAxis;
    
private:
    
    Viewport            m_ViewPort;
    int                 m_iAxisWidth;
    int                 m_iboxWidth = Globals::PianoRoll::initNoteWidth;
    int                 m_iNumBox = Globals::PianoRoll::initTimeStamps;
};
