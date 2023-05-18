using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Camera : MonoBehaviour
{
    /*
       메인 카메라 스크립트
       캐릭터를 따라다니면서 플레이어를 찍고있는 카메라
     */

    //카메라 타겟위치 = 플레이어
    public Transform m_Target;
    //카메라 위치
    private Transform m_CameraTr;
    //카메라 움직임 딜레이타임
    public float m_DelayTime = 2f;

    //카메라가 플레이어에서 떨어질 거리
    public float m_CmeraX;
    public float m_CmeraY;
    public float m_CmeraZ;

    void Start()
    {
        //카메라 위치정보 얻어옴
        m_CameraTr = GetComponent<Transform>();
    }

    void Update()
    {
        //카메라 이동부분
        Vector3 Pos = new Vector3(m_Target.position.x+ m_CmeraX, m_Target.position.y + m_CmeraY, m_Target.position.z - m_CmeraZ);
        m_CameraTr.position = /*Pos;*/ Vector3.Lerp(transform.position, Pos, Time.deltaTime * m_DelayTime);
    }


}
