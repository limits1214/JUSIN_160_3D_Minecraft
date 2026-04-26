#pragma once
#include "GameInstance.h"

NS_BEGIN(Engine)
class ENGINE_DLL CHelper final
{
public:
	template<typename F>
	static void MyHandleTreeDFS(const CHandle* rootHandle, F lambda)
	{

		std::stack<const CHandle*> s{};
		s.push(rootHandle);

		while (!s.empty())
		{
			if (const auto& node = s.top())
			{
				lambda(node);
				s.pop();

				if (auto* pObj = CGameInstance::Get().GetGameObjectByHandle(*node))
				{
					const auto& children = pObj->GetChildrenHandle();
					for (auto iter = children.rbegin(); iter != children.rend(); ++iter)
					{
						s.push(&(*iter));
					}
				}
				//const auto& children = node->GetChildrenNode();
				//for (auto iter = children.rbegin(); iter != children.rend(); ++iter)
				//{
				//	if ((*iter))
				//	{
				//		s.push((*iter));
				//	}
				//}
			}
		}
	}
};
NS_END
