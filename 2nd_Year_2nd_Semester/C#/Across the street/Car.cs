using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Car : MonoBehaviour
{
    public float MoveSpeed = 5f;    //이동물체의 속도

    void Start()
    {
        
    }

    private void OnCollisionEnter(Collision collision)  //충돌이 발생할시 실행
    {
        if(collision.gameObject.tag=="Player")              //충돌 발생한 오브젝트의 Tag가 Player면 실행
        {
            //GameObject.Destroy(collision.gameObject);  //충돌=삭제 x -> 충돌=죽음처리
            GameManager_.PlayerDie = true;                 //플레이어 사망 처리
        }
        
    }

    void Update()
    {
        if (GameManager_.PlayerDie)                         //플레이어가 죽은상태이면 아래의 내용을 실행 x
            return;

        Vector3 pos = this.transform.position;      //현재 자기의 위치를 변경하기위헤 변수에 넣어줌
        pos.x += MoveSpeed * Time.deltaTime;     //속도와 시간에따라 위치값을 수정해준다.
        this.transform.position = pos;                  //설정한 위치값을 자신의 위치값으로 수정한다. 

        if(transform.position.x<=-17.5)//일정 위치를 넘어가면 파괴
        {
            GameObject.Destroy(this.gameObject);
        }
        if(transform.position.x>=17)//일정 위치를 넘어가면 파괴
        {
            GameObject.Destroy(this.gameObject);
        }
    }
}
