using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerTrickleMove : MonoBehaviour
{
    //플레이어 물방울 움직임 ( MonsterTrickleMove 와 비슷 , 충돌대상만 다름) 

    //속도,힘,바운스횟수
    public float TrickleSpeed = 3f;
    public float velocity = 10f;
    int BounceNum = 1;

    //바운스여부
    bool IsBouns = false;

    Vector3 startPos;
    Vector3 Movement;
    Rigidbody rb;

    void Start()
    {
        BounceNum = PlayerData.BounceNum;
        Target();
    }
    void OnEnable()
    {
        rb=GetComponent<Rigidbody>();
        startPos = transform.position;
    }
    void Target()
    {
        GetComponent<Rigidbody>().AddForce(transform.forward * TrickleSpeed, ForceMode.VelocityChange);
    }
   
    void OnCollisionEnter(Collision collision)
    {
        // 충돌 지점
        Vector3 hitPos = collision.contacts[0].point;
        IsBouns = true;
        // 벽에 충돌시 튕기는용 입 사각, 반사각
        Vector3 incomingVec = hitPos - startPos;
        Vector3 reflectVec = Vector3.Reflect(incomingVec, collision.contacts[0].normal);

        Movement = reflectVec.normalized + Physics.gravity.normalized;
        startPos = transform.position;

        if (collision.transform.CompareTag("Wall"))
        {
            if (BounceNum <= 0)
            {
                Destroy(gameObject, 0.01f);
            }
        }
        if (collision.transform.CompareTag("Monster"))
        {
            Destroy(gameObject, 0.1f);
            //몹사망
            //Destroy(other.gameObject, 0.1f);
        }
    }
    void OnCollisionExit(Collision collision)
    {
        if (collision.transform.CompareTag("Wall"))
        {
            BounceNum--;
           
        }
    }
    void FixedUpdate()
    {
        if (null != rb&& IsBouns)
            rb.velocity = Movement * TrickleSpeed;

        if (BounceNum == 0)
            Destroy(gameObject, 3f);
    }
}

