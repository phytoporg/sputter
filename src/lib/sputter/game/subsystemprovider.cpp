#include "subsystemprovider.h"

using namespace sputter::game;

SubsystemProvider* SubsystemProvider::s_pSubsystemProvider = nullptr;

SubsystemProvider* SubsystemProvider::GetSubsystemProviderAddress()
{
    return s_pSubsystemProvider;
}

void SubsystemProvider::SetSubsystemProviderAddress(SubsystemProvider* pSubsystemProvider)
{
    s_pSubsystemProvider = pSubsystemProvider;
}