using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class River : MonoBehaviour
{
    void Start()
    {
        
    }
    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.tag == "Player")// 충돌한 오브젝트의 tag가 player면 죽음처리
        {
            GameManager_.PlayerDie = true; //강에 플레이어가 떨어졌을경우!
        }
    }
    void Update()
    {
        
    }
}
