#pragma once
#include <stack>
#include "Scene.h"

class CSceneManager
{
public:
    CSceneManager() = default;
    ~CSceneManager() = default;

    // ���� Ȱ��ȭ�� ���� ��ȯ
    CScene* GetCurrentScene() const {
        return m_pScenes.empty() ? nullptr : m_pScenes.top().get();
    }

    // ���ο� ���� ���ÿ� �߰�
    void PushScene(std::unique_ptr<CScene> pScene) {
        m_pScenes.push(std::move(pScene));
    }

    // ���� ���� �����ϰ� ���� ������ ���ư�
    void PopScene() {
        if (!m_pScenes.empty())
            m_pScenes.pop();
    }
private:
	std::stack<std::unique_ptr<CScene>> m_pScenes;
};

