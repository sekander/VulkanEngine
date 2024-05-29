#include "../../include/States/state_machine.h"
#include <cstdio>
//#include <stack>
#include <type_traits>


//std::stack<Ref>  StateMachine::_states;
//Ref              StateMachine::_newStates;
//bool             StateMachine::_isReplacing;
//bool             StateMachine::_isAdding;
//bool             StateMachine::_isRemoving;


void StateMachine::AddState(St newState, bool isReplacing)
{
    _isAdding = true;
    _isReplacing = isReplacing;

    _newStates = std::move(newState);
    if(!_isReplacing)
        _states.push(std::move(_newStates));

}

void StateMachine::RemoveState()
{
    printf("Deleting State\n");
    _isRemoving = true;
    _states.pop();
}

void StateMachine::Clear()
{
    printf("Clearing States\n");
    _newStates.reset();
}

void StateMachine::ProcessState()
{
    /*
    if(_isRemoving && !_states.empty())
    {
        printf("Removing State\n");
        _states.pop();
        _isRemoving = false;
    }
    */
    if(_isReplacing && _isAdding)
    {
        //if(!_states.empty())
        //    _states.pop();

        printf("Adding New State\n");
        _states.push(std::move(_newStates));
        _states.top();
        _isAdding = false;
    }

}

St &StateMachine::GetActiveState()
{
    return _states.top();    
}
