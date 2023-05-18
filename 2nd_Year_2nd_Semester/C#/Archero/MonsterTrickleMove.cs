using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonsterTrickleMove : MonoBehaviour
{
    //몬스터 발사체 설정

    //발사체 속도
    public float TrickleSpeed = 3f;
    //튕김 숫자
    public int BounceNum = 1;

    //충돌여부
    bool IsBouns = false;

    //반사용
    Vector3 startPos;
    Vector3 Movement;
    //rigidbody
    Rigidbody rb;

    void Start()
    {
        Target();
    }
    void OnEnable()
    {
        rb = GetComponent<Rigidbody>();
        startPos = transform.position;
    }
    
    void Target()
    {
        GetComponent<Rigidbody>().AddForce(transform.forward * TrickleSpeed, ForceMode.VelocityChange);
    }

    void OnCollisionEnter(Collision collision)
    {
        //충돌 지점
        Vector3 Hit = collision.contacts[0].point;
        IsBouns = true;

        //벽에 충돌시 튕기는용 입 사각, 반사각
        Vector3 StartVec = Hit - startPos;
        Vector3 reflectVec = Vector3.Reflect(StartVec, collision.contacts[0].normal);

        //이동vec
        Movement = reflectVec.normalized + Physics.gravity.normalized;
        startPos = transform.position;

        //충돌이 벽일경우
        if (collision.transform.CompareTag("Wall"))
        {
            if (BounceNum <= 0)
            {
                Destroy(gameObject, 0.01f);
            }
        }
        //충돌이 플레이어일경우
        if (collision.transform.CompareTag("Player"))
        {
            PlayerData.Player_Current_Hp -= 50;
            Destroy(gameObject, 0.1f);
        }
    }
    //충돌 나갈때 바운드횟수 감소
    void OnCollisionExit(Collision collision)
    {
        if (collision.transform.CompareTag("Wall"))
        {
            BounceNum--;

        }
    }
    void FixedUpdate()
    {
        if (null != rb && IsBouns)
            rb.velocity = Movement * TrickleSpeed;

        if (BounceNum == 0)
            Destroy(gameObject, 3f);
    }
}
