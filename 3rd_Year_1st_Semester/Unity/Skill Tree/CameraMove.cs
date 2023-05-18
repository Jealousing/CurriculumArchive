using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//카메라 움직임 스크립트
public class CameraMove : MonoBehaviour
{
    //줌(휠) 최대 최소
    float MAX_ZOOM_OUT = 150;
    float MAX_ZOOM_IN = 20;

    //카메라 정보
    Camera SkillTree_Camera;
    public float camera_speed = 10.0f; //카메라 속도

    //최대 좌표 설정
    int MaxPosX = 3000;
    int MaxPosZ = 2850;

    void Start()
    {
        //카메라 정보 받아옴
        SkillTree_Camera = GetComponent<Camera>();
    }
    void Update()
    {
        //드래그로 화면 이동
        Panning();
        //휠로 화면 인아웃
        ZoomInOut();
    }

    void Panning()//드래그 화면이동
    {
        //마우스 좌클릭
        if (Input.GetMouseButton(0))
        {
            //x 좌표가 제한 좌표를 넘었는지 확인
            if(SkillTree_Camera.transform.position.x< MaxPosX && SkillTree_Camera.transform.position.x > -MaxPosX) //안넘었으면
            {
                //드래그에 따른 카메라 이동
                SkillTree_Camera.transform.Translate(Vector3.right * -Input.GetAxis("Mouse X") * (camera_speed + (SkillTree_Camera.fieldOfView * SkillTree_Camera.fieldOfView / 100)));
            }
            else//넘었으면 좌표 보정
            {
                if (SkillTree_Camera.transform.position.x > MaxPosX)
                    SkillTree_Camera.transform.position=new Vector3(MaxPosX-100, SkillTree_Camera.transform.position.y, SkillTree_Camera.transform.position.z);
                if(SkillTree_Camera.transform.position.x<-MaxPosX)
                    SkillTree_Camera.transform.position = new Vector3(-MaxPosX+100, SkillTree_Camera.transform.position.y, SkillTree_Camera.transform.position.z);
            }

            //z 좌표가 제한 좌표를 넘었는지 확인
            if (SkillTree_Camera.transform.position.z < MaxPosZ && SkillTree_Camera.transform.position.z > -MaxPosZ)//안넘었으면
            {
                //드래그에 따른 카메라 이동
                SkillTree_Camera.transform.Translate(transform.up * -Input.GetAxis("Mouse Y") * (camera_speed + (SkillTree_Camera.fieldOfView * SkillTree_Camera.fieldOfView / 100)), Space.World);
            }
            else//좌표보정
            {
                if (SkillTree_Camera.transform.position.z > MaxPosZ)
                    SkillTree_Camera.transform.position = new Vector3(SkillTree_Camera.transform.position.x, SkillTree_Camera.transform.position.y, MaxPosZ-30);
                if (SkillTree_Camera.transform.position.z < -MaxPosZ)
                    SkillTree_Camera.transform.position = new Vector3(SkillTree_Camera.transform.position.x, SkillTree_Camera.transform.position.y, -MaxPosZ+30);

            }

        }
    }

    void ZoomInOut()//휠로 줌인아웃
    {
        //최대최소 줌 보정
        if (SkillTree_Camera.fieldOfView > MAX_ZOOM_OUT)
        {
            SkillTree_Camera.fieldOfView = MAX_ZOOM_OUT;
        }
        else if (SkillTree_Camera.fieldOfView < MAX_ZOOM_IN)
        {
            SkillTree_Camera.fieldOfView = MAX_ZOOM_IN;
        }
        else
        {
            //줌인아웃
            SkillTree_Camera.fieldOfView -= Input.GetAxis("Mouse ScrollWheel") * camera_speed;
        }
    }
}
