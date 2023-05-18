using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SubCamera : MonoBehaviour
{
    /*
     서브카메라 스크립트
    인벤토리 활성화가 되었을 경우 인벤토리에서 
     */

    //카메라 타겟위치
    public Transform m_Target;
   
    //카메라 위치
    private Transform m_CameraTr;
    //서브 카메라의 위치를 잡아주는 오브젝트의 위치
    public Transform m_CameraTr2;

    void Start()
    {
        //카메라 위치정보 얻어옴
        m_CameraTr = GetComponent<Transform>();
    }

    void Update()
    {
        //카메라 이동부분
        m_CameraTr.position = m_CameraTr2.position;
        m_CameraTr.transform.LookAt(m_Target);
    }
}
