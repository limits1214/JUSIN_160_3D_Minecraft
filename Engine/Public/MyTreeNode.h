#pragma once
#include "Engine_Defines.h"

NS_BEGIN(Engine)

template<typename N, typename F>
void MyTreeBFS(N* rootNode, F lambda)
{
    if (!rootNode) return;

    std::queue<N*> q{};
    q.push(rootNode);

    while (!q.empty())
    {
        if (auto*& node = q.front())
        {
            q.pop();
            lambda(node);
            for (auto& pChild : node->GetChildrenNode())
            {
                q.push(pChild);
            }
        }
    }
}

template<typename N, typename F>
void MyTreeDFS(N* rootNode, F lambda)
{
    if (!rootNode) return;

    std::stack<N*> s{};
    s.push(rootNode);

    while (!s.empty())
    {
        if (auto*& node = s.top())
        {
            s.pop();
            lambda(node);
            const auto& children = node->GetChildrenNode();
            for (auto iter = children.rbegin(); iter != children.rend(); ++iter)
            {
                if ((*iter))
                {
                    s.push((*iter));
                }
            }
        }
    }

}

template<typename N, typename F>
void MyTreeParentTraversal(N* pStartNode, F lambda)
{
    N* pNode = pStartNode;

    while (pNode)
    {
        lambda(pNode);
        pNode = pNode->GetParentNode();
    }
}

template<typename T>
class CMyTreeNode
{
protected:
    CMyTreeNode() = default;
    virtual ~CMyTreeNode()
    {
        SetParentNode(nullptr);
        std::for_each(m_pChildrenNode.begin(), m_pChildrenNode.end(),
            [](auto& pChild) { pChild->SetParentNode(nullptr); });
    }

protected:
    const T* GetParentNode() const { return m_pParentNode; }
    T* GetParentNode() { return m_pParentNode; }

public:
    const std::vector<T*>& GetChildrenNode() const { return m_pChildrenNode; }
    std::vector<T*>& GetChildrenNode() { return m_pChildrenNode; }

protected:
    void SetParentNode(T* pNewParentNode)
    {
        if (pNewParentNode)
        {
            if (m_pParentNode)
            {
                m_pParentNode->EraseChildNode(static_cast<T*>(this));
                m_pParentNode = pNewParentNode;
                m_pParentNode->m_pChildrenNode.push_back(static_cast<T*>(this));
            }
            else
            {
                m_pParentNode = pNewParentNode;
                m_pParentNode->m_pChildrenNode.push_back(static_cast<T*>(this));
            }
        }
        else
        {
            if (m_pParentNode)
            {
                m_pParentNode->EraseChildNode(static_cast<T*>(this));
                m_pParentNode = nullptr;
            }
        }
    }

protected:
    void EraseChildNode(T* pChildNode)
    {
        if (!pChildNode) return;

        auto iter = std::find(m_pChildrenNode.begin(), m_pChildrenNode.end(), pChildNode);
        if (iter != m_pChildrenNode.end())
        {
            m_pChildrenNode.erase(iter);
        }
    }

protected:
    T* m_pParentNode{};
    std::vector<T*> m_pChildrenNode{};
};

NS_END
