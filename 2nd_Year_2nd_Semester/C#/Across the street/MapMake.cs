using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapMake : MonoBehaviour
{
    public GameObject Player_Data;  //플레이어 정보
    public GameObject Target;          //복사할 맵의 정보

    float zpos;//좌표 저장용 변수
    void Start()
    {
        zpos = Target.transform.position.z; //시작할 경우 기준이 될 좌표 저장
    }
    void mapmake()//맵 생성
    {
        zpos += 581;    //맵의 z크기만큼 늘려줌
        GameObject tempobj = Target; //복사할 맵의 정보

        Vector3 pos1 = Target.transform.position;   //맵오브젝트의 좌표를 수정하기위해 변수 선언
        pos1.z = zpos;                                                //z좌표만 변경하여 설정 (이동방향 z)

        GameObject copyobj = GameObject.Instantiate(tempobj);       //오브젝트 복사
        copyobj.transform.position = pos1;                                          //복사한 오브젝트를 맵의 z좌표 차이만큼 옮기기

        Vector3 pos2 = this.transform.position;                                 //맵 생성의 기준을 빈 오브젝트의 위치에 가까워지면 실행되게 했기 때문에 이 오브젝트의 위치도 변경
        pos2.z += 581;                                                                          //이 역시 맵의 z 크기만큼 같이 이동
        this.transform.position = pos2;                                               //오브젝트 자기자신의 위치설정
    }
   
    void Update()
    {
        if (Vector3.Distance(Player_Data.transform.position, this.transform.position) <= 150f) //플레이어위치와 이 스크립트가 들어간 오브젝트의 위치를 비교
        {//가까워 질경우 맵생성
            mapmake();
        }
    }
}
