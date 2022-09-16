#include "scenestack.h"
#include "scene.h"

#include <sputter/system/system.h>

using namespace sputter::game;

SceneStack::SceneStack(IScene** ppSceneArray, uint32_t numScenes)
{
    if (numScenes > kMaxScenes)
    {
        system::LogAndFail("Creating too many scenes in scene stack.");
    }

    // Zero out m_ppSceneArray first
    memset(m_ppSceneArray, 0, sizeof(IScene*) * kMaxScenes);
    
    m_numScenes = numScenes;
    memcpy(m_ppSceneArray, ppSceneArray, sizeof(IScene*) * numScenes);
}

void SceneStack::Initialize()
{
    if (!m_ppSceneArray[m_currentSceneIndex])
    {
        system::LogAndFail("Current scene in scene stack is null");
    }

    m_ppSceneArray[m_currentSceneIndex]->Initialize();
}

bool SceneStack::PushToNextScene(IScene** ppNextSceneOut)
{
    if (m_currentSceneIndex == m_numScenes - 1)
    {
        return false;
    }
    
    IScene* pSceneOut = m_ppSceneArray[m_currentSceneIndex];
    m_currentSceneIndex++;
    IScene* pSceneIn = m_ppSceneArray[m_currentSceneIndex];

    pSceneOut->Uninitialize();
    pSceneIn->Initialize();

    if (ppNextSceneOut)
    {
        *ppNextSceneOut = pSceneIn;
    }
    
    return true;
}

bool SceneStack::PopToPreviousScene(IScene** ppPreviousSceneOut)
{
    if (m_currentSceneIndex == 0)
    {
        return false;
    }

    IScene* pSceneOut = m_ppSceneArray[m_currentSceneIndex];
    m_currentSceneIndex--;
    IScene* pSceneIn = m_ppSceneArray[m_currentSceneIndex];

    pSceneOut->Uninitialize();
    pSceneIn->Initialize();
    
    if (ppPreviousSceneOut)
    {
        *ppPreviousSceneOut = pSceneIn;
    }

    return true;
}

const IScene* SceneStack::GetCurrentScene()
{
    return m_ppSceneArray[m_currentSceneIndex];
}

void SceneStack::Tick(math::FixedPoint dt)
{
    if (!m_ppSceneArray[m_currentSceneIndex])
    {
        system::LogAndFail("Current scene in scene stack is null");
    }

    m_ppSceneArray[m_currentSceneIndex]->Tick(dt);
}

void SceneStack::Draw()
{
    if (!m_ppSceneArray[m_currentSceneIndex])
    {
        system::LogAndFail("Current scene in scene stack is null");
    }

    m_ppSceneArray[m_currentSceneIndex]->Draw();
}