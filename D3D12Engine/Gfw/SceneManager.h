#pragma once
#include <stack>
#include "Scene.h"

class CSceneManager
{
public:
    CSceneManager() = default;
    ~CSceneManager() = default;

    // 현재 활성화된 씬을 반환
    CScene* GetCurrentScene() const {
        return m_pScenes.empty() ? nullptr : m_pScenes.top().get();
    }

    // 새로운 씬을 스택에 추가
    void PushScene(std::unique_ptr<CScene> pScene) {
        m_pScenes.push(std::move(pScene));
    }

    // 현재 씬을 제거하고 이전 씬으로 돌아감
    void PopScene() {
        if (!m_pScenes.empty())
            m_pScenes.pop();
    }
private:
	std::stack<std::unique_ptr<CScene>> m_pScenes;
};

