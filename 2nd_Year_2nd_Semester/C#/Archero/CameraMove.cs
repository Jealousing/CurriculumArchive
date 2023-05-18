using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraMove : MonoBehaviour
{
    //카메라 움직임설정
    public Transform Target;            //카메라가 따라갈 플레이어
    private Transform CameraTr;     //카메라위치
    public float DelayTime = 2f;           //카메라 위치조정 딜레이
    void Start()
    {
        CameraTr = GetComponent<Transform>();
    }

    void FixedUpdate()
    {
        //카메라 위치조정
        Vector3 FixedPos = new Vector3(0 , Target.position.y + 20f, Target.position.z);
        CameraTr.position = Vector3.Lerp(transform.position, FixedPos, Time.deltaTime * DelayTime);
    }
}
