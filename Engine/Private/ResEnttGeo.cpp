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
