///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Copyright (c) 2018 Ethinza Inc., All rights reserved                      //
//                                                                           //
// This file contains TRADE SECRETs of ETHINZA INC. and is CONFIDENTIAL.     //
// Without written consent from ETHINZA INC., it is prohibited to disclose   //
// or reproduce its contents, or to manufacture, use or sell it in whole or  //
// part. Any reproduction of this file without written consent of ETHINZA    //
// INC. is a violation of the copyright laws and is subject to civil         //
// liability and criminal prosecution.                                       //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ethinza.h>

#include <functional>
#include <map>

typedef std::mem_fn Handler;

/**
 * @class StateMachine
 * @author Chirag
 * @date 30/12/2018
 * @file statemachine.h
 * @brief Basic statemachine utility class
 */
template<class Owner, class State, class Event>
class StateMachine
{
private:

    // One entry of state machine table
    struct StateEntry
    {
        Event event;
        auto handler;
        State next;
    }

    // Helper map
    typedef std::multimap<State, StateEntry> StateMap;

    // Owner object
    Owner* m_owner;
    // State table map
    StateMap m_stateTable;
    // Current state
    State m_currentState;
    // Handler when non matching event is found
    auto m_defaultHandler;

    StateMachine(const StateMachine& rhs) = delete;
    StateMachine& operator=(const StateMachine& rhs) = delete;

public:

    // ctor
    StateMachine(Owner* owner, Handler& defaultHandler)
    :   m_owner(owner),
        m_defaultHandler(defaultHandler)
    {
    }

    // dtor
    ~StateMachine()
    {
    }

    /**
     * @brief Adds new entry to state machine
     * @param current
     * @param event
     * @param handler
     * @param next
     */
    void AddState(State current, Event event, Handler& handler, State next)
    {
        StateEntry stateEntry;
        stateEntry.event = event;
        stateEntry.handler = handler;
        stateEntry.next = next;

        m_stateTable.insert(std::make_pair(current, stateEntry));

        if (m_stateTable.count() == 1) m_currentState = current;
    }

    /**
     * @brief Generated event is supplied to state machine
     * @param event
     */
    void ProcessEvent(Event event)
    {
        StateMap::iterator it = m_stateTable.lower_bound(m_currentState);
        StateMap::iterator itEnd = m_stateTable.upper_bound(m_currentState);
        bool eventFound = false;

        for (; (it != itEnd) && !eventFound; ++it)
        {
            if (it->second.event == event)
            {
                eventFound = true;
                m_currentState = it->first;
                it->second.handler(m_owner);
            }
        }

        if (!eventFound)
        {
            m_defaultHandler(m_owner);
        }
    }
};

