#include "ResEnttGeo.h"
NS_USING(Engine)
CResEnttGeo::CResEnttGeo(const _string& sPath)
	: CResource{ sPath }
{

}

CResEnttGeo::~CResEnttGeo()
{
}

void CResEnttGeo::RightToLeft()
{ 
    //uint32_t iCntCube{};
    for (auto& bone : m_entityGeometry.bones)
    {
        for (auto& cube : bone.cubes)
        {
            //++iCntCube;

            cube.uvSize = cube.size;

            // inflate 먼저
            cube.origin.x -= cube.inflate;
            cube.origin.y -= cube.inflate;
            cube.origin.z -= cube.inflate;
            cube.size.x += cube.inflate * 2.f;
            cube.size.y += cube.inflate * 2.f;
            cube.size.z += cube.inflate * 2.f;

            // Y축 180도와 동일 (X, Z 둘 다 반전 → winding 유지)
            cube.origin.z = -cube.origin.z - cube.size.z;
            cube.origin.x = -cube.origin.x - cube.size.x;

            cube.pivot.z = -cube.pivot.z;
            cube.pivot.x = -cube.pivot.x;

            cube.rotation.y = -cube.rotation.y; // Y회전만 부호 반전
        }
        bone.pivot.z = -bone.pivot.z;
        bone.pivot.x = -bone.pivot.x;

        if (!bone.locators.empty())
        {
            for (auto& locator : bone.locators)
            {
                locator.second.z = -locator.second.z;
                locator.second.x = -locator.second.x;
            }
        }
    }
}

void CResEnttGeo::Scaling()
{
    float fScale = 0.0625f;  // 원하는 스케일값

    for (auto& bone : m_entityGeometry.bones)
    {
        // pivot 스케일
        bone.pivot.x *= fScale;
        bone.pivot.y *= fScale;
        bone.pivot.z *= fScale;

        // 각 큐브의 스케일
        for (auto& cube : bone.cubes)
        {
            cube.origin.x *= fScale;
            cube.origin.y *= fScale;
            cube.origin.z *= fScale;

            cube.size.x *= fScale;
            cube.size.y *= fScale;
            cube.size.z *= fScale;

            cube.inflate *= fScale;

            cube.pivot.x *= fScale;
            cube.pivot.y *= fScale;
            cube.pivot.z *= fScale;
        }

        // locators 스케일 (있으면)
        for (auto& [name, pos] : bone.locators)
        {
            pos.x *= fScale;
            pos.y *= fScale;
            pos.z *= fScale;
        }
    }
}

void CResEnttGeo::BoneSorting()
{
    std::vector<ENTITY_BONE> bones{};

    // 1. 이름 → 인덱스 임시 맵
    std::unordered_map<std::string, size_t> tempLookup;
    for (size_t i = 0; i < m_entityGeometry.bones.size(); ++i)
        tempLookup.emplace(m_entityGeometry.bones[i].name, i);

    // 2. 위상 정렬
    std::vector<size_t> sorted;
    std::vector<bool> visited(m_entityGeometry.bones.size(), false);

    std::function<void(size_t)> visit = [&](size_t idx)
        {
            if (visited[idx]) return;
            visited[idx] = true;

            // 부모 먼저
            const auto& parentName = m_entityGeometry.bones[idx].parent;
            if (!parentName.empty())
            {
                auto it = tempLookup.find(parentName);
                if (it != tempLookup.end())
                    visit(it->second);
            }

            sorted.push_back(idx);
        };

    for (size_t i = 0; i < m_entityGeometry.bones.size(); ++i)
        visit(i);

    // 3. 정렬된 순서로 bones 구성
    for (size_t i = 0; i < sorted.size(); ++i)
    {
        const auto& bone = m_entityGeometry.bones[sorted[i]];
        bones.push_back(bone);
        //auto b = CEntityModelBone{ bone.name };
        //b.SetPivot(bone.pivot);
        //b.SetParentName(bone.parent);
        //m_BonesLookup.emplace(bone.name, m_Bones.size());
        //m_Bones.push_back(b);
    }
    m_entityGeometry.bones = bones;
}
