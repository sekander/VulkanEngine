#pragma once

#include <cstdio>
#include <memory>
#include <stack>

#include "state.h"

using St = std::unique_ptr<State>;

class StateMachine{

public:    
    StateMachine(){}
    ~StateMachine(){
        printf("Deleting MAchine\n");
	
    }
    void AddState(St newState, bool isReplacing);
    void RemoveState();
    void ProcessState();
    void Clear();
    St &GetActiveState();

    //All states in stack

private:

    std::stack<St> _states;
    //Latest state to add
    St _newStates;

    bool _isRemoving, _isReplacing, _isAdding;


};
