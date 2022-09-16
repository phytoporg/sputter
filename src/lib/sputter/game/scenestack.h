#pragma once

#include <cstdint>
#include <sputter/math/fixedpoint.h>

namespace sputter { namespace game {
    class IScene;

    class SceneStack
    {
    public:
        // A more robust system might configure some kind of graph, but we'll start with an actual stack where
        // the array passed in here is ordered, and pushing and popping in scene transitions simply goes up 
        // and down the stack.
        SceneStack(IScene** ppSceneArray, uint32_t numScenes);

        void Initialize();

        bool PushToNextScene(IScene** ppNextSceneOut = nullptr);
        bool PopToPreviousScene(IScene** ppPreviousSceneOut = nullptr);

        void Tick(math::FixedPoint dt);
        void Draw();

    private:
        uint32_t m_currentSceneIndex = 0;

        static const uint32_t kMaxScenes = 8;
        IScene* m_ppSceneArray[kMaxScenes];
        uint32_t m_numScenes = 0;
    };
}}