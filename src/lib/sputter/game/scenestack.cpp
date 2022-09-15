#include "scenestack.h"
#include "scene.h"

using namespace sputter::game;

SceneStack::SceneStack(IScene** ppSceneArray, uint32_t numScenes)
    : m_ppSceneArray(ppSceneArray), m_numScenes(numScenes)
{}

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
    m_currentSceneIndex--
    IScene* pSceneIn = m_ppSceneArray[m_currentSceneIndex];

    pSceneOut->Uninitialize();
    pSceneIn->Initialize();
    
    if (ppPreviousSceneOut)
    {
        *ppPreviousSceneOut = pSceneIn;
    }

    return true;
}

void SceneStack::Tick(math::FixedPoint dt)
{
    m_ppSceneArray[m_currentSceneIndex]->Tick(dt);
}