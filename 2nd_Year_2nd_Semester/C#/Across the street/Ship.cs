using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ship : MonoBehaviour
{
    bool ParentPlayer = false; //연결되었는지 설정

    public float MoveSpeed = 5f;    //배의 이동속도
    void Start()
    {
        ParentPlayer = false; //처음엔 연결x
    }
    private void OnCollisionEnter(Collision collision)  //충돌할경우
    {
        if (collision.transform.tag == "Player"&&!ParentPlayer) //tag가 player일 경우 && 연결되지 않았을경우
        {
            collision.transform.SetParent(transform);   //충돌한 오브젝트와 연결
            ParentPlayer = true;    //연결
        }
    }
    private void OnCollisionExit(Collision collision)   //충돌에서 벗어날경우
    {
        if (collision.transform.tag == "Player")    //tag가 플레이어였으면
        {
            collision.transform.SetParent(null);    //연결해제
            collision.transform.localScale = new Vector3(0.7f, 0.7f, 0.7f); //무슨이유인지는 몰라도 scale값이 달라져 다시설정.
            ParentPlayer = false;   //연결해제 알림
        }
    }

    void Update()
    {
        if (GameManager_.PlayerDie) //플레이어가 죽어있을경우 return
            return;

        Vector3 pos = this.transform.position;  //위치변경을 위한 백터설정
        pos.x += MoveSpeed * Time.deltaTime;    //설정 속도에 따라 증가
        this.transform.position = pos;  //위치 설정

        if (transform.position.x <= -17.5)//일정 값을 넘어가면 파괴
        {
            GameObject.Destroy(this.gameObject);

        }
        if (transform.position.x >= 17)//일정 값을 넘어가면 파괴
        {
            GameObject.Destroy(this.gameObject);
        }

    }
}
