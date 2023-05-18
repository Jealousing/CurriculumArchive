using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Camera : MonoBehaviour
{
    public Transform Target;                //카메라가 따라갈 타겟의 위치정보
    private Transform CameraTr;        //카메라의 위치정보
    public float DelayTime=2f;               //카메라가 자연스럽게 따라가기 위한 딜레이

    public float LeftVision = -35f;         //Q를 통해 좌측을 볼때의 시야각
    public float RightVision = 20f;         //E를 통해 우측을 볼때의 시야각

    void Start()
    {
        CameraTr = GetComponent<Transform>(); //카메라 Transform받아옴
    }

    void LateUpdate()
    {
        
    }

    void Update()
    {
        Vector3 Pos=new Vector3(Target.position.x + 1f, Target.position.y + 3f, Target.position.z - 3f);    //플레이어의 위치기준으로 위치값 설정
        CameraTr.position = Vector3.Lerp(transform.position, Pos, Time.deltaTime * DelayTime);          //받은 위치값 기준으로 딜레이 타임을 걸어 부드러운 카메라 움직임을 설정

        if(Input.GetKeyDown(KeyCode.E))//E를 누르면 우측으로 시야회전
        {
            CameraTr.rotation = Quaternion.Euler(30, RightVision, 0);
        }
        if(Input.GetKeyUp(KeyCode.E))//E키에서 손을 때면 원래시야로 돌아옴
        {
            CameraTr.rotation = Quaternion.Euler(30, -15, 0);
        }
        if (Input.GetKeyDown(KeyCode.Q))//Q를 누르면 좌측으로 시야회전
        {
            CameraTr.rotation = Quaternion.Euler(30, LeftVision, 0);
        }
        if (Input.GetKeyUp(KeyCode.Q))//Q키에서 손을 때면 원래시야로 돌아옴
        {
            CameraTr.rotation = Quaternion.Euler(30, -15, 0);
        }
    }
}
